#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include "optparse.h"
#include "xdrCustom.h"
#include "msgService.h"
#include "zlog.h"
#include "hex2byte.h"
#include "clientTool.h"
#include "baseType.h"
#define VERSION "0.1"

#define VERSION "0.1"

// mqtt 客户端句柄
static XL_CHANNEL_CTX_T *s_ctx;

sint32             iSocketClient;
struct sockaddr_in tSocketServerAddr;

struct timeval sendStart;
struct timeval sendDone;
struct timeval recvDone;

struct timeval r1;
struct timeval r2,r3;
sint8 *url;


sint8 *cmd;
sint8 host[20];
sint8 *checkInfo;
sint8 *wbuf;
sint8 *infoNums;
sint8 *minorDIs;
sint8 *dataITDs;
uint16 port;
uint8  logicalDeviceNum;
uint16 infoNum;
uint32 dataTag;
uint32 queryTimeType;
uint32 startTime;
uint32 endTime;
uint32 storageTime;
uint32 sortType;
uint32 majorDI;
uint32 minorDINum;
uint32 minorDI;
uint32 gcount =10000;
uint32 async = 0;
uint32 gackcount = 0;
uint32 startindex = 0;
uint16 dataTypeID;
uint32 majorDIOptinal = 1;

uint32 handerNum;
uint32 dataBufLen;
uint32 minorDIOptinal;
uint32 screenType;
uint32 storageIndex;

uint8  prm         = 0;
uint8  priority    = 0;
uint16 _index      = 0;
uint16 label       = 0;
sint8 *chdes       = "smiOS";
sint8 *source      = "advClientTool";
sint8 *destination = "M-dbCenter";
uint32 tag;
uint32 timeout        = 50;
uint32 bigpackageTest = 0;

uint32 subscribeInfo = 0xf;
uint32 version       = 1;
uint32 year          = 2019;
uint32 month         = 12;
uint32 day           = 20;
uint32 hour          = 0;
uint32 minute        = 0;
uint32 second        = 0;
sint8 *qname         = "dbCenter";
uint32 eventNum      = 0;
sint8 *sub           = 0;

uint8  bigpackageTestBuf[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER) * 2] = {0};
sint8  rc[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER) * 3]                = {};
uint8  buf[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER) * 3];
sint8  writebuf[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER) * 3];
uint8  cmdbuf[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER) * 10];
uint8  tmpbuf[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER) * 3];
uint32 tmpbuf_len  = 0;
uint8  flag_tmpbuf = 0;

// mqtt 连接状态
XL_CHANNEL_CONNECT_STATUS_E s_mqtt_status;

#define LOGIN_STATE_INIT 0
#define LOGIN_STATE_START 1
#define LOGIN_STATE_WAIT 2
#define LOGIN_STATE_DONE 3

// 登录标志
static uint32 g_flag_login = LOGIN_STATE_INIT;

struct optparse_long longopts[] = {

    {"cmd", 'a', OPTPARSE_REQUIRED},
    {"port", 'b', OPTPARSE_REQUIRED},
    {"host", 'c', OPTPARSE_REQUIRED},
    {"logicalDeviceNum", 'd', OPTPARSE_REQUIRED},
    {"infoNum", 'e', OPTPARSE_REQUIRED},
    {"dataTag", 'f', OPTPARSE_REQUIRED},
    {"wbuf", 'g', OPTPARSE_REQUIRED},
    {"queryTimeType", 'h', OPTPARSE_REQUIRED},
    {"startTime", 'i', OPTPARSE_REQUIRED},
    {"endTime", 'j', OPTPARSE_REQUIRED},
    {"storageTime", 'k', OPTPARSE_REQUIRED},
    {"sortType", 'l', OPTPARSE_REQUIRED},
    {"majorDI", 'm', OPTPARSE_REQUIRED},
    {"minorDINum", 'n', OPTPARSE_REQUIRED},
    {"minorDI", 'o', OPTPARSE_REQUIRED},
    {"minorDIs", 'p', OPTPARSE_REQUIRED},
    {"handerNum", 'q', OPTPARSE_REQUIRED},
    {"checkInfo", 'r', OPTPARSE_REQUIRED},
    {"dataBufLen", 's', OPTPARSE_REQUIRED},
    {"minorDIOptinal", 't', OPTPARSE_REQUIRED},
    {"screenType", 'u', OPTPARSE_REQUIRED},
    {"storageIndex", 'v', OPTPARSE_REQUIRED},
    {"dataITDs", 'w', OPTPARSE_REQUIRED},
    {"infoNums", 'x', OPTPARSE_REQUIRED},
    {"gcount", 'y', OPTPARSE_REQUIRED},
    {"async", 'z', OPTPARSE_REQUIRED},
    {"prm", 'A', OPTPARSE_REQUIRED},
    {"priority", 'B', OPTPARSE_REQUIRED},
    {"index", 'C', OPTPARSE_REQUIRED},
    {"label", 'D', OPTPARSE_REQUIRED},
    {"source", 'E', OPTPARSE_REQUIRED},
    {"destination", 'F', OPTPARSE_REQUIRED},
    {"tag", 'G', OPTPARSE_REQUIRED},
    {"timeout", 'H', OPTPARSE_REQUIRED},
    {"chdes", 'J', OPTPARSE_REQUIRED},
    {"big", 'I', OPTPARSE_REQUIRED},
    {"subscribeInfo", 'K', OPTPARSE_REQUIRED},
    {"version", 'L', OPTPARSE_REQUIRED},
    {"year", 'M', OPTPARSE_REQUIRED},
    {"month", 'N', OPTPARSE_REQUIRED},
    {"day", 'O', OPTPARSE_REQUIRED},
    {"hour", 'P', OPTPARSE_REQUIRED},
    {"minute", 'Q', OPTPARSE_REQUIRED},
    {"second", 'R', OPTPARSE_REQUIRED},
    {"qname", 'S', OPTPARSE_REQUIRED},
    {"sub", 'T', OPTPARSE_REQUIRED},
    {"url", 'U', OPTPARSE_REQUIRED},
    {"SV", 'V', OPTPARSE_NONE},
    {"majorDIOptinal", 'W', OPTPARSE_REQUIRED},
    {"dataTypeID", 'X', OPTPARSE_REQUIRED},
    {0}};

typedef struct dbCenterTest
{
    uint8 nNum;
    uint8 nValue[100];
}DATA_UNITY_T;

typedef struct
{
    uint8  logicid;
    uint32 majorDI;
    uint32 minorDIOptinal;
    uint32 minorDI[200];
    uint32 startTime;
    uint32 endTime;
    uint32 storageTime;
    uint16 inforNum;
    sint8  checkInfo[6];
    sint16  datalen;
    sint8  databuf[256];
}RECORD_DATA_T;

uint32 strtou32(char *str) 
{
    uint32 temp=0;
    uint32 fact=1;
    unsigned char len=strlen(str);  
    unsigned char i;
    for(i=len;i>0;i--)
    {
        temp+=((str[i-1]-0x30)*fact);
        fact*=10;
    }
    return temp;
}

sint8 read_normal_data(uint8 logicid, uint16 infornum, uint32 datatag, DATA_UNITY_T *data)
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050010;

    uint8 *p = buf;
    push_u8(&p, logicid);
    push_u16(&p, infornum);
    push_u32(&p, datatag);
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        // printf("msg->prm = 0x%x\n", obj.prm);
        // printf("msg->priority = 0x%x\n", obj.priority);
        // printf("msg->index = 0x%x\n", obj.index);
        // printf("msg->label = 0x%x\n", obj.label);
        // printf("msg->source = %s\n", obj.source);
        // printf("msg->destination = %s\n", obj.destination);
        // printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 OPTIONAL = pop_u8(&payload);
        uint8 *databuf;
        uint32 len = pop_nbyte(&payload, &databuf);
        // printf("OPTIONAL=[0x%x]\n", OPTIONAL);

        Hex2Str(databuf, rc, len);
        // printf("BUFLEN=[%d]\n", len);
        // printf("BUF= [%s]\n", rc);
        //hdzlog_debug(data, len);
        data->nNum = len;
        memcpy(data->nValue, databuf, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

sint8 write_normal_data(uint8 logicid, uint16 infornum, uint32 datatag,uint16 dataID, DATA_UNITY_T *data)
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050011;

    uint8 *p = buf;
    push_u8(&p, logicid);
    push_u16(&p, infornum);
    push_u32(&p, datatag);
    push_u16(&p, dataID);
    // printf("dataTypeID = %u\n", dataTypeID);
    // printf("dataTag = %u\n", dataTag);
    push_nbyte(&p, data->nValue, data->nNum);

    // sint32 wbufLen = strlen(wbuf);
    // HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    // push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        // printf("msg->prm = 0x%x\n", obj.prm);
        // printf("msg->priority = 0x%x\n", obj.priority);
        // printf("msg->index = 0x%x\n", obj.index);
        // printf("msg->label = 0x%x\n", obj.label);
        // printf("msg->source = %s\n", obj.source);
        // printf("msg->destination = %s\n", obj.destination);
        // printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK = pop_u8(&payload);
        // printf("ACK= [0x%x]\n", ACK);
    }
    else
    {
        dzlog_error("timeout");
    }
}

sint8 start_read_record_data(RECORD_DATA_T *readData, uint8 timeType, uint8 sortType, uint32 *handlerNum, uint32 *recordNum)
{
    sint32 i;
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050012;

    uint8 *p = buf;
    push_u8(&p, readData->logicid);
    push_u16(&p, readData->inforNum);
    push_u8(&p, timeType);
    if (timeType != 255)
    {
        push_u32(&p, readData->startTime);
        push_u32(&p, readData->endTime);
    }
    push_u8(&p, sortType);
    push_u8(&p, majorDIOptinal);  //获取次DI选项
    if (majorDIOptinal == 1)
    {
        push_u32(&p, readData->majorDI);
    }
    // push_u32(&p,readData->majorDI);

        push_sequnce_length(&p, readData->minorDI[0]);
        push_u32(&p, readData->minorDI[1]);

    //uint32 tmp_minnorDI[200];
    //printf("minorDIs[%s]\n", minorDIs);
    //minorDINum = str2intbuf(minorDIs, ",", tmp_minnorDI, 200);

    /* if (readData->minorDIOptinal > 0)
    {
        push_u8(&p, readData->minorDIOptinal);
        for (i = 0; i < readData->minorDIOptinal; i++)
        {
            push_u32(&p, readData->minorDI[i]);
            //printf("tmp_minnorDI[%d] = [%d]\n", i, tmp_minnorDI[i]);
        }
    }
    else
    {
        push_u8(&p, 0);
    } */
    // push_u32(&p, readData->minorDI[0]);

    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // msg_build(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        // msg_parser(rsp.payload, recvn, &obj);
        /*
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        */
        uint8 *payload = obj.payload;
        uint32 HANDERNUM = pop_u32(&payload);
        uint32 RECORDNUM = pop_u32(&payload);
        /*
        printf("HANDERNUM= [%u]\n", HANDERNUM);
        printf("RECORDNUM= [%d]\n", RECORDNUM);
        */
        //hdzlog_debug(data, len);
        *handlerNum = HANDERNUM;
        *recordNum = RECORDNUM;
        return 0;
    }
    else
    {
        // dzlog_error("timeout");
        return -1;
    }
}

sint8 step_read_record(uint32 handleNum, uint32 dataLen, uint8 *checkInfo, RECORD_DATA_T *readResult)
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050013;
    sint16 i        = 0;

    uint8 *p = buf;
    push_u32(&p, handleNum);
    push_u8(&p, 1);
    if (1)
    {
        sint8 checkBuf[100] = {0};
        sint32 checkLen     = strlen(checkInfo);
        // printf("checkInfo len=[%d]\n", checkLen);
        HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
        push_nbyte(&p, checkBuf, checkLen >> 1);
    }


    //sint8 checkBuf[100] = {0};
    //sint32 checkLen = strlen(checkInfo);
    //HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
    // push_fixbyte(&p, checkInfo, 6);
    // push_nbyte(&p, checkInfo, 6);
    // push_u32(&p, dataLen);

    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // msg_build(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        // hdzlog_debug(rsp.payload, recvn);
        pro_parser_msg(rsp.payload, recvn, &obj);
        // msg_parser(rsp.payload, recvn, &obj);
        /*
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        */
        uint8 *payload = obj.payload;

        uint32 TIME_OPTINAL = 0;
        uint32 OPTINAL = pop_u8(&payload);
        //printf("OPTINAL=[%d]\n", OPTINAL);
        
        if (OPTINAL == 0)
        {
            return;
        }
        uint32 INFONUM = pop_u16(&payload);
        uint8 *octet          = NULL;
        sint16 sequnce_length = pop_sequnce_length(&payload);
        octet = payload;

        for (i = 0; i < sequnce_length; i++)
        {
            uint32 MAJORDI        = 0;
            uint32 MAJORDIOPTINAL = 0;
            MAJORDIOPTINAL        = pop_u8(&octet);
            if (MAJORDIOPTINAL)     
            {
                MAJORDI = pop_u32(&octet);
            }
            uint32 MINORDI = 0;

            MINORDI = pop_u32(&octet);

            uint8 *data       = 0;
            uint32 DATABUFLEN = pop_nbyte(&octet, &data);
            sint8 rc[4096] = {};
            // printf("DATABUFLEN = %d",DATABUFLEN);
            // hdzlog_debug(data,DATABUFLEN);
            Hex2Str(data, rc, DATABUFLEN);
            // printf("DATABUFLEN=[%d]\n", DATABUFLEN);
            // printf("BUF=[%s]\n", rc);
        }
        TIME_OPTINAL = pop_u8(&octet);
        if (TIME_OPTINAL)
        {

            uint32 STARTTIME   = pop_u32(&octet);
            uint32 ENDTIME     = pop_u32(&octet);
            uint32 STORAGETIME = pop_u32(&octet);
            // printf("STARTTIME=[%d]\n", STARTTIME);
            // printf("ENDTIME=[%d]\n", ENDTIME);
            // printf("STORAGETIME=[%d]\n", STORAGETIME);
        }
    }
    else
    {
        // dzlog_error("timeout");
    }
        return 0;
}

sint8 stop_read_record_data(uint32 handlerNum)
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050014;

    uint8 *p = buf;
    push_u32(&p, handlerNum);
    //printf("handerNum %u\n",handerNum);
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);

    // msg_build(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        // msg_parser(rsp.payload, recvn, &obj);
        /*
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        */
        uint8 *payload = obj.payload;
        uint8 ACK = pop_u8(&payload);
        // printf("ACK=[0x%x]\n", ACK);
        if (1 == ACK)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        // dzlog_error("timeout");
        return -1;
    }
}

sint8 write_record_data(RECORD_DATA_T *writeData, uint16 *recordNum)
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050015;

    uint8 *p = buf;
    push_u8(&p, writeData->logicid);
    if (majorDIOptinal)
    {
        push_u8(&p, 1);
        push_u32(&p, writeData->majorDI);
    }
    else
    {
        push_u8(&p, 0);
    }
    push_u32(&p, writeData->minorDI[1]);
    push_u16(&p, dataTypeID);



    // push_u32(&p, writeData->majorDI);
    /* if (writeData->minorDIOptinal)
    {
        push_u8(&p, 1);
        push_u32(&p, writeData->minorDI[0]);
    }
    else
    {
        push_u8(&p, 0);
    } */
    push_u32(&p, writeData->startTime);
    push_u32(&p, writeData->endTime);
    push_u32(&p, writeData->storageTime);
    push_u16(&p, writeData->inforNum);
    sint8 checkBuf[100] = {0};
    sint32 checkLen     = strlen(writeData->checkInfo);
    HexStrToByte(writeData->checkInfo, checkBuf, checkLen & 0xfffffffe);
    push_nbyte(&p, checkBuf, checkLen >> 1);
    // push_nbyte(&p, timeNow, strlen(timeNow));


    // sint8 checkBuf[100] = {0};
    // HexStrToByte(writeData->checkInfo, checkBuf, 6 & 0xfffffffe);
    // push_nbyte(&p, checkBuf, 6);
    // push_fixbyte(&p, writeData->checkInfo, 6);

    // push_nbyte(&p, checkInfo, 6);
    //sint8 writebuf[1024];
    //sint32 wbufLen = strlen(wbuf);
    //HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writeData->databuf, writeData->datalen);
    
    obj.length = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // msg_build(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        // msg_parser(rsp.payload, recvn, &obj);
        /*
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        */
        uint8 *payload = obj.payload;
        uint16 STORENUM = pop_u16(&payload);
        //printf("STORENUM= [%d]\n", STORENUM);
        *recordNum = STORENUM;
        return 0;
    }
    else
    {
        // dzlog_error("timeout");
        return -1;
    }
}

sint32 init_socket()
{
    struct timeval tv_out;
    tv_out.tv_sec = timeout;
    tv_out.tv_usec = 0;

    iSocketClient = socket(AF_INET, SOCK_DGRAM, 0);
    tSocketServerAddr.sin_family = AF_INET;
    tSocketServerAddr.sin_port = htons(port); /* host to net, sint16 */

    memset(tSocketServerAddr.sin_zero, 0, 8);
    inet_aton(&host, &tSocketServerAddr.sin_addr);
    setsockopt(iSocketClient, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
}

sint32 udp_send(void *buf, sint32 buflen)
{
    socklen_t AddrLen = sizeof(struct sockaddr);
    sendto(iSocketClient, buf, buflen, 0,
           (const struct sockaddr *)&tSocketServerAddr, AddrLen);
}

sint32 udp_recv(void *buf, sint32 bufMaxLen)
{
    socklen_t AddrLen = sizeof(struct sockaddr);
    sint32 n = recvfrom(iSocketClient, buf, bufMaxLen, 0, (struct sockaddr *)&tSocketServerAddr, &AddrLen);
    if (n > 0)
    {
        //hdzlog_debug(buf, n);
    }
    return n;
}

sint32 main(void)
{
    uint32 i = 0,j = 0;
    struct timeval start,end,t1,t2,t3,t4;
    long dif_sec,dif_usec;
    DATA_UNITY_T testData;
    sint8 *p;

    dzlog_init(NULL, "clientTool");
    // dzlog_info("hello");

    snprintf(host, sizeof(host),"10.12.42.199");
    port = 8598;
    init_socket();


    printf("Write 10000 Normal(param.real.bkp) Data Start...\n");
    logicalDeviceNum = 0;
    dataTag = 0x99;
    dataTypeID = 3;
    gettimeofday(&start,NULL);
    for(i = 0; i < 50000; i++)
    {
        infoNum = i;
        sprintf(testData.nValue,"NormTest[%u]",i);
        testData.nNum = strlen(testData.nValue);
        
        write_normal_data(logicalDeviceNum, infoNum, dataTag,dataTypeID, &testData);

    }
    gettimeofday(&end,NULL);
    dif_sec = end.tv_sec - start.tv_sec;
    dif_usec = end.tv_usec - start.tv_usec;
    if(dif_usec<0)
    {
        dif_sec --;
        dif_usec = start.tv_usec - end.tv_usec;
    }
    printf("Write 10000 Normal(param.real.bkp) Data Finished, Used:%ld s  %ld us.\n", dif_sec,dif_usec);

    
    printf("Read 10000 Normal(param.real.bkp) Data Start...\n");
    DATA_UNITY_T readData;
    uint8 testValue[256];
    
    gettimeofday(&start,NULL);
    for(i = 0; i < 50000; i++)
    {
        // logicalDeviceNum = 0;
        infoNum = i;
        // dataTag = 60000200;

        read_normal_data(logicalDeviceNum, infoNum, dataTag, &readData);
        
        // else 
        // {
            // strncpy(&p,readData.nNum,strlen(readData.nNum)-1);
            // printf(" Normal data: %s \n",readData.nValue);
        // }
    }
    gettimeofday(&end,NULL);
    dif_sec = end.tv_sec - start.tv_sec;
    dif_usec = end.tv_usec - start.tv_usec;
    if(dif_usec<0)
    {
        dif_sec --;
        dif_usec = start.tv_usec - end.tv_usec;
    }
    printf("Read 10000 Normal(param.real.bkp) Data Finished, Used:%ld s  %ld us.\n", dif_sec,dif_usec);


    /* printf("Write 1000000 Record Data Start...\n");
    RECORD_DATA_T recordData;
    uint16 recordNum;
    recordData.logicid = 0x00;
    recordData.inforNum = 0x01;
    recordData.majorDI  = 1;
    recordData.minorDIOptinal = 1;
    recordData.minorDI[0]  = 1;
    recordData.minorDI[1]  = 4;

    recordData.checkInfo[0] = 0x41;
    recordData.checkInfo[1] = 0x42;
    recordData.checkInfo[2] = 0x43;
    recordData.checkInfo[3] = 0x44;
    recordData.checkInfo[4] = 0x45;
    recordData.checkInfo[5] = 0x46;
    gettimeofday(&start,NULL);
    for(i = 0; i < 50000; i++)
    {
        recordData.startTime    = 0;
        recordData.endTime      = 100000;
        recordData.storageTime  = i + 1000;

        sprintf(recordData.databuf,"RecordTest[%u]",i);
        recordData.datalen = strlen(recordData.databuf);
        
        if(0 != write_record_data( &recordData, &recordNum))
        {
            printf("Write Record Data Error!");
            continue;
        }
    }
    gettimeofday(&end,NULL);
    dif_sec = end.tv_sec - start.tv_sec;
    dif_usec = end.tv_usec - start.tv_usec;
    if(dif_usec<0)
    {
        dif_sec --;
        dif_usec = start.tv_usec - end.tv_usec;
    }
    printf("Write 1000000 Record Data Finished, Used:%ld s  %ld us.\n", dif_sec,dif_usec);


    RECORD_DATA_T recordResult;
    uint32 ReadRecordHandlerNum = 0;
    uint32 ReadRecordNum = 0;
    do
    {
        printf("start Read Record Data Start\n");

    gettimeofday(&t1,NULL);

        if(0 != start_read_record_data( &recordData, 2, 255, &ReadRecordHandlerNum, &ReadRecordNum))
        {
            printf("start Read Record Data  Error! \n");
            break;
        }
        gettimeofday(&t2,NULL);
        printf("start Read Record Data, HandlerNum = %u, RecordNum = %u\n", ReadRecordHandlerNum, ReadRecordNum);


    gettimeofday(&start,NULL);
        for(i = 0; i < ReadRecordNum; i++)
        {
            if(0 != step_read_record(ReadRecordHandlerNum, 100, recordData.checkInfo, &recordResult))
            {
                printf("Step Read Record Data Error!\n");
                continue;
            }
        }

        printf("Step Read Record Data Finish\n");
    gettimeofday(&t3,NULL);
        
        if(0 != stop_read_record_data(ReadRecordHandlerNum))
        {
            printf("Stop Read Record Data Stop Error!\n");
        }
    } while (0);
    gettimeofday(&t4,NULL);
    gettimeofday(&end,NULL);

    dif_sec = t2.tv_sec - t1.tv_sec;
    dif_usec = t2.tv_usec - t1.tv_usec;
    if(dif_usec < 0)
    {
        dif_sec --;
        dif_usec = 1000000 - t1.tv_usec + t2.tv_usec;
    }
    printf("start read 50000 Record Data Finished, Used:%ld s  %ld us.\n", dif_sec,dif_usec);

    dif_sec = t4.tv_sec - t3.tv_sec;
    dif_usec = t4.tv_usec - t3.tv_usec;
    if(dif_usec < 0)
    {
        dif_sec --;
        dif_usec = 1000000 - t3.tv_usec + t4.tv_usec;
    }
    printf("stop read 50000 Record Data Finished, Used:%ld s  %ld us.\n", dif_sec,dif_usec);

    dif_sec = end.tv_sec - start.tv_sec;
    dif_usec = end.tv_usec - start.tv_usec;
    if(dif_usec < 0)
    {
        dif_sec --;
        dif_usec = 1000000 - start.tv_usec + end.tv_usec;
    }
    printf("step read 50000 Record Data Finished, Used:%ld s  %ld us.\n", dif_sec,dif_usec);
 */

    return 0;
}
