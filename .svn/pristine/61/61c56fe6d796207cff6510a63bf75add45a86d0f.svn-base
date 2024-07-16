#include "baseType.h"
#include "clientTool.h"
#include "hex2byte.h"
#include "msgService.h"
#include "optparse.h"
#include "timeHelper.h"
#include "xdrCustom.h"
#include "xlChannel.h"
#include "zlog.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "argv.h"

#define VERSION "0.1"

FILE *fp = NULL;

// mqtt �ͻ��˾��
static XL_CHANNEL_CTX_T *s_ctx;

sint32             iSocketClient;
struct sockaddr_in tSocketServerAddr;

struct timeval sendStart;
struct timeval sendDone;
struct timeval recvDone;
struct timeval r1,r2,r3;
sint8 *url;


sint8 *cmd;
sint8 *host;
sint8 *checkInfo;
sint8 *wbuf;
sint8 *infoNums;
sint8 *minorDIs;
sint8 *dataITDs;
uint16 port;
uint8  logicalDeviceNum;
uint16 infoNum;
uint32 dataTag;
uint8 queryTimeType;
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
uint32 timeout        = 60;
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

// mqtt ����״̬
XL_CHANNEL_CONNECT_STATUS_E s_mqtt_status;

#define LOGIN_STATE_INIT 0
#define LOGIN_STATE_START 1
#define LOGIN_STATE_WAIT 2
#define LOGIN_STATE_DONE 3

// ��¼��־
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

#define BUF_COUNT 10
#define WRITE_COUNT 1000
char testBuf[BUF_COUNT];
struct timeval writeTime[WRITE_COUNT];
float timeS[WRITE_COUNT];
float timeUS[WRITE_COUNT];
float timeR[WRITE_COUNT];

int bufWrite()
{
    int i;    
    for(i =0;i < BUF_COUNT; i ++)
    {
        testBuf[i] = 0x31;
    }
    return 0;
}
int fileWrite()
{
    int i;
    for(i = 0;i < gcount; i ++)
    {
        timeS[i] = writeTime[i+1].tv_sec - writeTime[i].tv_sec;
        timeUS[i] = writeTime[i+1].tv_usec - writeTime[i].tv_usec;
        if(timeUS[i] < 0)
        {
            timeS[i] --;
            timeUS[i] = 1000000 + timeUS[i];
        }
        timeR[i] = timeS[i] * 1000 + timeUS[i]/1000;
    }

    fp = fopen("db.csv","at+");
    if(fp == NULL)
    {
        printf("file open error! \n");
        return -1;    
    }

    for(i = 0;i < gcount; i ++)
    {
        fprintf(fp,"%d,%f \n",i,timeR[i]);
    }
    fclose(fp);
    fp = NULL;
    return 0;
}    

uint32 strtou32(char *str)
{
    uint32        temp = 0;
    uint32        fact = 1;
    unsigned char len  = strlen(str);
    unsigned char i;
    for (i = len; i > 0; i--)
    {
        temp += ((str[i - 1] - 0x30) * fact);
        fact *= 10;
    }
    return temp;
}

#if 0
static sint32
build_msg(MSG_OBJ_T *msgObj, sint8 *payload, sint32 payload_len, RAW_OBJ_T *rsp)
{
    uint8 *p                = rsp->payload;
    p[MSG_PRM_OFFSET]       = (msgObj->prm & 0x01) | (msgObj->priority << 1);
    p[MSG_INDEX_OFFSET]     = msgObj->index >> 8;
    p[MSG_INDEX_OFFSET + 1] = msgObj->index & 0xff;
    p[MSG_LABEL_OFFSET]     = msgObj->label >> 8;
    p[MSG_LABEL_OFFSET + 1] = msgObj->label & 0xff;
    sint8 *source_addr      = p + MSG_SOURCE_OFFSET;
    uint8  source_len       = strlen(msgObj->source);
    strncpy(source_addr, msgObj->source, source_len);
    sint8 *des_addr = source_addr + source_len + 1;
    uint8  des_len  = strlen(msgObj->destination);
    strncpy(des_addr, msgObj->destination, des_len);
    uint8 *tag_addr = des_addr + des_len + 1;
    tag_addr[3]     = msgObj->tag & 0xff;
    tag_addr[2]     = (msgObj->tag >> 8) & 0xff;
    tag_addr[1]     = (msgObj->tag >> 16) & 0xff;
    tag_addr[0]     = (msgObj->tag >> 24) & 0xff;
    uint8 *len_add  = tag_addr + 4;
    uint8 *xdr_payload_addr;

    if (payload_len < XDR_SIGNEL_LEN)
    {
        len_add[0]       = payload_len;
        xdr_payload_addr = len_add + MSG_PYLOAD_OFFSET_1;
    }
    else
    {
        len_add[0]       = 0x82;
        len_add[1]       = (payload_len >> 8) & 0xff;
        len_add[2]       = payload_len & 0xff;
        xdr_payload_addr = len_add + MSG_PYLOAD_OFFSET_3;
    }
    memcpy(xdr_payload_addr, payload, payload_len);
    rsp->length = xdr_payload_addr - p + payload_len;
    return rsp->length;
}

static sint32 pro_parser_msg(sint8 *rxbuf, sint32 rxbuf_len, MSG_OBJ_T *msgObj)
{
    msgObj->prm         = GET_PRM(rxbuf);
    msgObj->priority    = GET_PRIORITY(rxbuf);
    msgObj->index       = GET_INDEX(rxbuf);
    msgObj->label       = GET_LABEL(rxbuf);
    msgObj->source      = GET_SOURCE(rxbuf);
    msgObj->destination = GET_DESTINATION(rxbuf);
    sint8 *tag_addr     = GET_TAG_ADDR(msgObj->destination);
    msgObj->tag         = GET_TAG(tag_addr);
    sint8 *len_addr     = GET_LENGTH_ADDR(tag_addr);
    msgObj->length      = GET_LENGTH(len_addr);
    msgObj->payload = len_addr + (msgObj->length < 0x80 ? MSG_PYLOAD_OFFSET_1 :
                                                          MSG_PYLOAD_OFFSET_3);
}
#endif

// appע��
void app_register(void)
{
    MSG_OBJ_T obj;
    sint32    recvn;
    uint32    i;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010010;

    uint8        payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *      p   = payloadBuf;
    MQT_PLUGIN_T mqt = {0};

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    push_u32(&p, getpid());
    strcpy((sint8 *)mqt.comName, source);
    // mqt.subscribeInfo   = subscribeInfo;

    uint32 tmp_subInfo[1024];
    uint32 n = hexStr2U32buf(sub, ",", tmp_subInfo, 1024);
    printf("n = %d\n", n);

    mqt.subr.subscribeInfoNum = n;
    for (i = 0; i < (n > APP_MAX_SUB_NUM ? APP_MAX_SUB_NUM : n); i++)
    {
        mqt.subr.subscribeInfo[i] = tmp_subInfo[i];
        printf("tmp_subInfo[%d] = %08x\n", i, mqt.subr.subscribeInfo[i]);
    }

    mqt.version         = version;
    mqt.dataTime.year   = year;
    mqt.dataTime.month  = month;
    mqt.dataTime.day    = day;
    mqt.dataTime.hour   = hour;
    mqt.dataTime.minute = minute;
    mqt.dataTime.second = second;

    push_mqt_plugin(&p, &mqt);

    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);

    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8  ACK     = pop_u8(&payload);
        printf("ACK=%d\n", ACK);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void unregister_app(void)
{
    MSG_OBJ_T obj;
    sint32    recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010011;

    uint8  payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *p = payloadBuf;
    // MQT_PLUGIN_T mqt = {0};

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};

    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);

    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8  ACK     = pop_u8(&payload);
        printf("ACK=%d\n", ACK);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

//��ѯapp
void query_app(void)
{
    MSG_OBJ_T obj;
    sint32    recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010012;

    uint8  payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *p = payloadBuf;

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};

    build_msg(&obj, payloadBuf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);

    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        hdzlog_debug(rsp.payload, recvn);
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8  NUM     = pop_u8(&payload);
        int i          = 0;
        int j          = 0;
        printf("=====NUM[%d]=====\n", NUM);
        {
            MQT_PLUGIN_T mqt;

            for (i = 0; i < NUM; i++)
            {
                pop_mqt_plugin(&payload, &mqt);
                printf("=====app_info=====\n");
                printf("mqt.comName:%s\n", mqt.comName);
                printf("mqt.subscribeInfoNum:0x%x\n",
                       mqt.subr.subscribeInfoNum);
                for (j = 0; j < mqt.subr.subscribeInfoNum; j++)
                {
                    printf("mqt.subscribeInfo[%u]:0x%08x\n", j,
                           mqt.subr.subscribeInfo[j]);
                }
                printf("mqt.version:%u\n", mqt.version);
                printf("mqt.dataTime.year:%u\n", mqt.dataTime.year);
                printf("mqt.dataTime.month:%u\n", mqt.dataTime.month);
                printf("mqt.dataTime.day:%u\n", mqt.dataTime.day);
                printf("mqt.dataTime.hour:%u\n", mqt.dataTime.hour);
                printf("mqt.dataTime.minute:%u\n", mqt.dataTime.minute);
                printf("mqt.dataTime.second:%u\n", mqt.dataTime.second);
            }
        }
        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

//��ѯָ��app
void query_app_by_name(void)
{
    MSG_OBJ_T obj;
    sint32    recvn;
    uint32    i;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010013;

    uint8        payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *      p   = payloadBuf;
    MQT_PLUGIN_T mqt = {0};

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};

    push_nbyte(&p, (uint8 *)qname, strlen(qname));

    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);

    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        // uint8  ACK     = pop_u8(&payload);
        // printf("ACK=%d\n", ACK);
        printf("=====app_info=====\n");
        pop_mqt_plugin(&payload, &mqt);
        printf("mqt.comName:%s\n", mqt.comName);
        printf("mqt.subscribeInfoNum:0x%x\n", mqt.subr.subscribeInfoNum);
        for (i = 0; i < mqt.subr.subscribeInfoNum; i++)
        {
            printf("mqt.subscribeInfo[%u]:0x%08x\n", i,
                   mqt.subr.subscribeInfo[i]);
        }
        printf("mqt.version:%u\n", mqt.version);
        printf("mqt.dataTime.year:%u\n", mqt.dataTime.year);
        printf("mqt.dataTime.month:%u\n", mqt.dataTime.month);
        printf("mqt.dataTime.day:%u\n", mqt.dataTime.day);
        printf("mqt.dataTime.hour:%u\n", mqt.dataTime.hour);
        printf("mqt.dataTime.minute:%u\n", mqt.dataTime.minute);
        printf("mqt.dataTime.second:%u\n", mqt.dataTime.second);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}


////////////======================================///////////////////

void read_normal_data()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050010;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum);
    push_u32(&p, dataTag);
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    dzlog_debug("===>1");
    build_msg(&obj, buf, obj.length, &rsp);
    dzlog_debug("===>2");
    hdzlog_debug(rsp.payload, rsp.length);
    dzlog_debug("===>3");
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 OPTIONAL = pop_u8(&payload);
        uint8 *data;
        uint32 len = pop_nbyte(&payload, &data);
        printf("OPTIONAL=[0x%x]\n", OPTIONAL);

    memset(rc,0,strlen(rc));

        Hex2Str(data, rc, len);
        printf("BUFLEN=[%d]\n", len);
        printf("BUF= [%s]\n", rc);
        hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void write_normal_data()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050011;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum);
    push_u32(&p, dataTag);
    push_u16(&p, dataTypeID);
    printf("dataTypeID = %u\n", dataTypeID);
    printf("dataTag = %u\n", dataTag);
    sint32 wbufLen = strlen(wbuf);
    HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length  = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK      = pop_u8(&payload);
        printf("ACK= [0x%x]\n", ACK);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void start_read_record_data()
{
    sint32 i;
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050012;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum);

    push_u8(&p, queryTimeType);
    if (queryTimeType != 255)
    {
        push_u32(&p, startTime);
        push_u32(&p, endTime);
    }
    push_u8(&p, sortType);

    push_u8(&p, majorDIOptinal);  //��ȡ��DIѡ��
    if (majorDIOptinal == 1)
    {
        push_u32(&p, majorDI);
    }

    uint32 tmp_minnorDI[200];
    printf("sortType[%d]\n", sortType);
    printf("minorDIs[%s]\n", minorDIs);
    minorDINum = str2intbuf(minorDIs, ",", tmp_minnorDI, 200);
    printf("minorDINum:%d\n", tmp_minnorDI[0]);
    if (tmp_minnorDI[0] > 0)
    {
        uint16 diNum = tmp_minnorDI[0];
        push_sequnce_length(&p, diNum);
        for (i = 0; i < diNum; i++)
        {
            push_u32(&p, tmp_minnorDI[i + 1]);
            printf("tmp_minnorDI[%d] = [%d]\n", i, tmp_minnorDI[i + 1]);
        }
    }
    else
    {
        push_u8(&p, 0);
    }

    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload   = obj.payload;
        uint32 HANDERNUM = pop_u32(&payload);
        uint32 RECORDNUM = pop_u32(&payload);
        printf("HANDERNUM= [%u]\n", HANDERNUM);
        printf("RECORDNUM= [%d]\n", RECORDNUM);
        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void step_read_record_old()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050013;

    uint8 *p = buf;
    push_u32(&p, handerNum);

    sint8 checkBuf[100] = {0};
    sint32 checkLen     = strlen(checkInfo);
    HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
    push_fixbyte(&p, checkBuf, 6);
    // push_nbyte(&p, checkInfo, 6);
    push_u32(&p, dataBufLen);
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;

        uint32 OPTINAL = pop_u8(&payload);

        uint32 STARTTIME   = pop_u32(&payload);
        uint32 ENDTIME     = pop_u32(&payload);
        uint32 STORAGETIME = pop_u32(&payload);
        uint32 INFONUM     = pop_u16(&payload);
        uint8 *octet       = NULL;
        uint32 OCTETSTRLEN = pop_nbyte(&payload, &octet);

        uint32 MAJORDI        = pop_u32(&octet);
        uint32 MINORDIOPTINAL = pop_u8(&octet);
        uint32 MINORDI        = 0;
        if (MINORDIOPTINAL)
        {
            MINORDI = pop_u32(&octet);
        }
        uint32 DATABUFLEN = pop_u32(&octet);

        uint8 *data;
        // printf("DATABUFLEN = %d",DATABUFLEN);
        pop_fixbyte(&octet, &data, DATABUFLEN);
        hdzlog_debug(data,DATABUFLEN);
    memset(rc,0,strlen(rc));
        Hex2Str(data, rc, DATABUFLEN);

        printf("OPTINAL=[%d]\n", OPTINAL);
        if (OPTINAL)
        {
            printf("STARTTIME=[%d]\n", STARTTIME);
            printf("ENDTIME=[%d]\n", ENDTIME);
            printf("STORAGETIME=[%d]\n", STORAGETIME);
            printf("INFONUM=[%d]\n", INFONUM);
            printf("OCTETSTRLEN=[%d]\n", OCTETSTRLEN);

            printf("MAJORDI=[%d]\n", MAJORDI);
            printf("MINORDIOPTINAL=[%d]\n", MINORDIOPTINAL);
            printf("MINORDI=[%d]\n", MINORDI);
            printf("DATABUFLEN=[%d]\n", DATABUFLEN);
            printf("BUF=[%s]\n", rc);
        }
    }
    else
    {
        dzlog_error("timeout");
    }
}
void step_read_record()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050013;
    sint16 i        = 0;

    uint8 *p = buf;
    push_u32(&p, handerNum);
    push_u8(&p, majorDIOptinal);
    if (majorDIOptinal)
    {
        sint8 checkBuf[100] = {0};
        sint32 checkLen     = strlen(checkInfo);
        HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
        push_nbyte(&p, checkBuf, checkLen >> 1);
    }

    // push_u32(&p, dataBufLen);
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);ch
    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;

        uint32 TIME_OPTINAL = 0;
        uint32 OPTINAL      = pop_u8(&payload);
        printf("OPTINAL=[%d]\n", OPTINAL);

        if (OPTINAL == 0)
        {
            return;
        }

        uint32 INFONUM = pop_u16(&payload);
        printf("INFONUM=[%d]\n", INFONUM);
        uint8 *octet          = NULL;
        sint16 sequnce_length = pop_sequnce_length(&payload);
        // uint32 OCTETSTRLEN = pop_nbyte(&payload, &octet);
        octet = payload;

        printf("sequnce_length = %d \n", sequnce_length);
        // printf("OCTETSTRLEN = %d\n",OCTETSTRLEN);
        for (i = 0; i < sequnce_length; i++)
        {
            uint32 MAJORDI        = 0;
            uint32 MAJORDIOPTINAL = 0;
            MAJORDIOPTINAL        = pop_u8(&octet);
            if (MAJORDIOPTINAL)
            {
                MAJORDI = pop_u32(&octet);
            }

            printf("MAJORDI=[%d]\n", MAJORDI);
            // uint32 MINORDIOPTINAL = pop_u8(&octet);
            // printf("MINORDIOPTINAL=[%d]\n", MINORDIOPTINAL);
            uint32 MINORDI = 0;
            // if (MINORDIOPTINAL)
            // {
            MINORDI = pop_u32(&octet);
            printf("MINORDI=[%d]\n", MINORDI);
            // }
            // uint32 DATABUFLEN = pop_u32(&octet);
            uint8 *data       = 0;
            uint32 DATABUFLEN = pop_nbyte(&octet, &data);
            // pop_fixbyte(&octet, &data,DATABUFLEN);
            Hex2Str(data, rc, DATABUFLEN);
            printf("DATABUFLEN = %d", DATABUFLEN);
            printf("BUF=[%s]\n", rc);
            // hdzlog_debug(data,DATABUFLEN);
        }

        TIME_OPTINAL = pop_u8(&payload);
        if (TIME_OPTINAL)
        {
            uint32 STARTTIME   = pop_u32(&payload);
            uint32 ENDTIME     = pop_u32(&payload);
            uint32 STORAGETIME = pop_u32(&payload);
            printf("STARTTIME=[%d]\n", STARTTIME);
            printf("ENDTIME=[%d]\n", ENDTIME);
            printf("STORAGETIME=[%d]\n", STORAGETIME);
        }
    }
    else
    {
        dzlog_error("timeout");
    }
}
void stop_read_record_data()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050014;

    uint8 *p = buf;
    push_u32(&p, handerNum);
    printf("handerNum %u\n", handerNum);
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK      = pop_u8(&payload);
        printf("ACK=[0x%x]\n", ACK);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void write_record_data()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050015;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);

    if (majorDIOptinal)
    {
        push_u8(&p, 1);
        push_u32(&p, majorDI);
    }
    else
    {
        push_u8(&p, 0);
    }
    push_u32(&p, minorDI);
    push_u16(&p, dataTypeID);

    push_u32(&p, startTime);
    push_u32(&p, endTime);
    push_u32(&p, storageTime);
    push_u16(&p, infoNum);

    printf("infoNum = %u\n", infoNum);

    sint8 checkBuf[100] = {0};
    sint32 checkLen     = strlen(checkInfo);
    HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
    push_nbyte(&p, checkBuf, checkLen >> 1);
    // push_fixbyte(&p, checkBuf, 6);

    // push_nbyte(&p, checkInfo, 6);
    sint32 wbufLen = strlen(wbuf);
    HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length  = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload  = obj.payload;
        uint32 STORENUM = pop_u32(&payload);
        printf("STORENUM= [%u]\n", STORENUM);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void update_record_data()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050016;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);

    if (majorDIOptinal)
    {
        push_u8(&p, 1);
        push_u32(&p, majorDI);
    }
    else
    {
        push_u8(&p, 0);
    }
    push_u32(&p, minorDI);
    // push_u32(&p, startTime);
    // push_u32(&p, endTime);
    // push_u32(&p, storageTime);
    push_u8(&p, screenType);
    if (screenType == 1)
    {
        push_u32(&p, storageTime);
    }
    else
    {
        push_u32(&p, storageIndex);
    }

    push_u16(&p, infoNum);

    sint8 checkBuf[100] = {0};
    sint32 checkLen     = strlen(checkInfo);
    HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
    push_nbyte(&p, checkBuf, checkLen >> 1);
    // push_fixbyte(&p, checkBuf, 6);

    sint32 wbufLen = strlen(wbuf);
    HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length  = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK      = pop_u8(&payload);
        printf("ACK= [0x%x]\n", ACK);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void get_storage_num()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050020;

    uint8 *p = buf;
    //׷��
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum);
    //

    if (majorDIOptinal)
    {
        push_u8(&p, 1);
        push_u32(&p, majorDI);
    }
    else
    {
        push_u8(&p, 0);
    }
    push_u32(&p, minorDI);
    push_u8(&p, screenType);
    if (screenType == 1)
    {
        push_u32(&p, startTime);
        push_u32(&p, endTime);
    }

    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload    = obj.payload;
        uint32 STORAGENUM = pop_u32(&payload);

        printf("STORAGENUM=[%d]\n", STORAGENUM);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void get_depth_num()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050021;

    uint8 *p = buf;

    if (majorDIOptinal)
    {
        push_u8(&p, 1);
        push_u32(&p, majorDI);
    }
    else
    {
        push_u8(&p, 0);
    }
    push_u32(&p, minorDI);
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload   = obj.payload;
        uint32 DEPTHNUMM = pop_u32(&payload);

        printf("DEPTHNUMM=[%d]\n", DEPTHNUMM);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void data_init()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050030;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);

    if (majorDIOptinal)
    {
        push_u8(&p, 1);
        push_u16(&p, dataTypeID);
    }
    else
    {
        push_u8(&p, 0);
    }
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK      = pop_u8(&payload);

        printf("ACK=[%d]\n", ACK);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void data_init_by_parameter()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050031;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum);
    push_u8(&p, sortType);
    if (sortType == 0)
    {
        push_u16(&p, dataTypeID);
    }
    else if (sortType == 1)
    {
        printf("majorDI = 0x%x\n", majorDI);

        printf("majorDIOptinal = 0x%x\n", majorDIOptinal);
        if (majorDIOptinal)
        {
            printf("minorDI = 0x%x\n", minorDI);
            push_u8(&p, 1);
            push_u32(&p, majorDI);
        }
        else
        {
            push_u8(&p, 0);
        }
        push_u32(&p, minorDI);
    }

    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK      = pop_u8(&payload);

        printf("ACK=[%d]\n", ACK);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void factory_Reset()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050032;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);

    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK      = pop_u8(&payload);

        printf("ACK=[%d]\n", ACK);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void start_write_record_data()
{
    sint32 i;
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050042;
    uint8 *p        = buf;

    obj.length  = p - buf;
    obj.payload = buf;
    // dzlog_error("test");
    hdzlog_debug(buf, 6);
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload   = obj.payload;
        uint32 HANDERNUM = pop_u32(&payload);
        // uint32 RECORDNUM = pop_u32(&payload);
        printf("HANDERNUM= [%u]\n", HANDERNUM);
        // printf("RECORDNUM= [%d]\n", RECORDNUM);
        // hdzlog_debug(data, len);
    }
    else
    {
        printf("timeout");
    }
}

void step_write_record()
{
    int i = 0;
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050043;

    uint8 *p = buf;
    ///
    push_u32(&p, handerNum);

    push_u8(&p, logicalDeviceNum);

    if (majorDIOptinal)
    {
        push_u8(&p, 1);
        push_u32(&p, majorDI);
    }
    else
    {
        push_u8(&p, 0);
    }
    push_u32(&p, minorDI);
    push_u32(&p, startTime);
    push_u32(&p, endTime);
    push_u32(&p, storageTime);
    push_u16(&p, infoNum);
    /////
    sint8 checkBuf[100] = {0};
    sint32 checkLen     = strlen(checkInfo);
    HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
    // push_fixbyte(&p, checkBuf, 6);
    push_nbyte(&p, checkInfo, checkLen >> 1);
    //
    sint32 wbufLen = strlen(wbuf);
    HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length  = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);

    printf("majorDI = %d \n", majorDI);
    printf("minorDI = %d \n", minorDI);
    printf("infoNum = %d\n", infoNum);
    printf("startTime = %d\n", startTime);
    printf("endTime = %d \n", endTime);
    printf("storageTime = %d\n", storageTime);
    printf("check data:");
    for (i = 0; i < checkLen; i++)
    {
        printf("data= %02x: ", checkBuf[i]);
    }
    printf("\n");
    printf("bin data:");
    for (i = 0; i < wbufLen; i++)
    {
        printf("data = %02x:", writebuf[i]);
    }
    printf("\n");

    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;

        uint32 rc = pop_u32(&payload);

        printf("rc=[%d]\n", rc);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void stop_write_record_data()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050044;

    uint8 *p = buf;
    push_u32(&p, handerNum);
    printf("handerNum %u\n", handerNum);
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK      = pop_u8(&payload);
        printf("ACK=[0x%x]\n", ACK);
    }
    else
    {
        dzlog_error("timeout");
    }
}

////////////======================================///////////////////

void client_set_subscription()
{
    MSG_OBJ_T obj;
    sint32    recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010020;

    uint8 *p = buf;

    uint32 tmp_subInfo[1024];

    uint32 n = hexStr2U32buf(sub, ",", tmp_subInfo, 1024);

    uint32 i = 0;
    if (n < 0x80)
    {
        push_u8(&p, n);
    }
    else
    {
        (p)[0] = 0x82;
        (p)[1] = ((n >> 8) & 0xff);
        (p)[2] = n & 0xff;
        (p) += 3;
    }

    for (i = 0; i < n; i++)
    {
        push_u32(&p, tmp_subInfo[i]);
    }

    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8  ACK     = pop_u8(&payload);

        printf("ACK=[%d]\n", ACK);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void client_get_subscription()
{
    sint32             i;
    MSG_OBJ_T          obj;
    sint32             recvn;
    APP_SUB_INFO_RAW_T sub        = {0};
    uint32             subInfoLen = 0;
    uint32 *           pSubInfo   = NULL;

    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010022;

    uint8 *p = buf;

    push_nbyte(&p, (uint8 *)qname, strlen(qname));

    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;

        subInfoLen = pop_nint(&payload, (uint8 **)(&pSubInfo));

        if (subInfoLen > 0)
        {
            subInfoLen =
                subInfoLen > APP_MAX_SUB_NUM ? APP_MAX_SUB_NUM : subInfoLen;
            sub.subscribeInfoNum = subInfoLen;
            printf("subscribeInfoNum =[%d]\n", sub.subscribeInfoNum);
            for (i = 0; i < subInfoLen; i++)
            {
                sub.subscribeInfo[i] = ntohl(pSubInfo[i]);  //�ֽ���ת��
                printf("subscribeInfo[%u] =[0x%08x]\n", i,
                       sub.subscribeInfo[i]);
            }
        }

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void client_set_unsubscription()
{
    MSG_OBJ_T obj;
    sint32    recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010021;

    uint8 *p = buf;

    uint32 tmp_subInfo[1024];

    uint32 n = hexStr2U32buf(sub, ",", tmp_subInfo, 1024);

    uint32 i = 0;
    if (n < 0x80)
    {
        push_u8(&p, n);
    }
    else
    {
        (p)[0] = 0x82;
        (p)[1] = ((n >> 8) & 0xff);
        (p)[2] = n & 0xff;
        (p) += 3;
    }

    for (i = 0; i < n; i++)
    {
        push_u32(&p, tmp_subInfo[i]);
    }

    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8  ACK     = pop_u8(&payload);

        printf("ACK=[%d]\n", ACK);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

#if 1
sint32 init_socket()
{
    struct timeval tv_out;
    tv_out.tv_sec  = timeout;
    tv_out.tv_usec = 0;

    iSocketClient = socket(AF_INET, SOCK_DGRAM, 0);
    if (iSocketClient < 0)
    {
        printf("create socket fail!\n");
    }
    tSocketServerAddr.sin_family = AF_INET;
    tSocketServerAddr.sin_port   = htons(port); /* host to net, sint16 */

    memset(tSocketServerAddr.sin_zero, 0, 8);
    inet_aton(host, &tSocketServerAddr.sin_addr);
    setsockopt(iSocketClient, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
}
#endif

#if 0
sint32  xl_send(void *buf, sint32 buflen)
{
    gettimeofday(&sendStart, NULL);
    socklen_t AddrLen = sizeof(struct sockaddr);
    sendto(iSocketClient, buf, buflen, 0,
           (const struct sockaddr *)&tSocketServerAddr, AddrLen);
    gettimeofday(&sendDone, NULL);
}
#endif

#if 0
sint32  xl_recv(void *buf, sint32 bufMaxLen)
{
    socklen_t AddrLen = sizeof(struct sockaddr);
    sint32    n       = recvfrom(iSocketClient, buf, bufMaxLen, 0,
                        (struct sockaddr *)&tSocketServerAddr, &AddrLen);
    if (n > 0)
    {
        gettimeofday(&recvDone, NULL);
        hdzlog_debug(buf, n);
    }
    return n;
}
#endif

/*
void getSysTime(struct timeval *tv)
{
    gettimeofday(tv, NULL);
}

*/
sint32 xl_recv(void *buf, sint32 bufMaxLen)
{
    struct timeval rc;
    struct timeval now;
    gettimeofday(&rc, NULL);
    for (;;)
    {
        gettimeofday(&now, NULL);
        if (now.tv_sec - rc.tv_sec > timeout)
        {
            return -1;
        }

        if (flag_tmpbuf == 1)
        {
            flag_tmpbuf = 0;
            memcpy(buf, tmpbuf, tmpbuf_len);
            // dzlog_debug("tmpbuf_len=%d", tmpbuf_len);
            return tmpbuf_len;
        }
    };
    return -1;
}

sint32 xl_send(void *buf, sint32 bufLen)
{
    return xl_channel_write(s_ctx, buf, bufLen);
}


void reboot(void)
{
    MSG_OBJ_T obj;
    sint32    recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010030;

    uint8  payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *p = payloadBuf;
    // MQT_PLUGIN_T mqt = {0};

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};

    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);

    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8  ACK     = pop_u8(&payload);
        printf("ACK=%d\n", ACK);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void get_power_state(void)
{
    MSG_OBJ_T obj;
    sint32    recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010031;

    uint8  payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *p = payloadBuf;
    // MQT_PLUGIN_T mqt = {0};

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};

    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);

    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload     = obj.payload;
        uint8  POWER_STATE = pop_u32(&payload);
        printf("POWER_STATE=%d\n", POWER_STATE);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void get_ethernet_state(void)
{
    MSG_OBJ_T obj;
    sint32    recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00010032;

    uint8  payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *p = payloadBuf;
    // MQT_PLUGIN_T mqt = {0};

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};

    push_nbyte(&p, (uint8 *)qname, strlen(qname));

    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);

    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload   = obj.payload;
        uint8  ETH_STATE = pop_u32(&payload);
        printf("ETH_STATE=%d\n", ETH_STATE);

        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void test_write_record_data(char * timeNow)
{
    MSG_OBJ_T obj;
    sint32    recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050015;

    uint8 *p = buf;

#if 0
    printf("logicalDeviceNum = %d\n", logicalDeviceNum);
    printf("majorDIOptinal = %d\n", majorDIOptinal);
    printf("majorDI = %d\n", majorDI);
    printf("minorDI = %d\n", minorDI);
    printf("startTime = %d\n", startTime);
    printf("endTime = %d\n", endTime);
    printf("storageTime = %d\n", storageTime);
    printf("infoNum = %d\n", infoNum);
    // printf("infoNum = %d\n", infoNum);
#endif

    push_u8(&p, logicalDeviceNum);

    if (majorDIOptinal)
    {
        push_u8(&p, 1);
        push_u32(&p, majorDI);
    }
    else
    {
        push_u8(&p, 0);
    }

    push_u32(&p, minorDI);
    push_u16(&p, dataTypeID);

    push_u32(&p, startTime++);
    push_u32(&p, endTime);
    push_u32(&p, storageTime);
    push_u16(&p, infoNum);
    sint8 checkBuf[100] = {0};
    sint32 checkLen     = strlen(checkInfo);
    // printf("checkLen = %d\n", checkLen);
    HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
    push_nbyte(&p, checkBuf, checkLen >> 1);
    // push_fixbyte(&p, checkBuf, 6);


    // push_nbyte(&p, checkInfo, 6);
    // sint32 wbufLen = strlen(wbuf);
    // HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, timeNow, strlen(timeNow));
    obj.length  = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);
    gettimeofday(&sendStart,NULL);

    xl_send(rsp.payload, rsp.length);
    gettimeofday(&sendDone, NULL);

    if (async == 0){
#if 0
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        // pro_parser_msg(rsp.payload, recvn, &obj);
        // printf("msg->prm = 0x%x\n", obj.prm);
        // printf("msg->priority = 0x%x\n", obj.priority);
        // printf("msg->index = 0x%x\n", obj.index);
        // printf("msg->label = 0x%x\n", obj.label);
        // printf("msg->source = %s\n", obj.source);
        // printf("msg->destination = %s\n", obj.destination);
        // printf("msg->tag = 0x%x\n", obj.tag);
        // uint8 *payload  = obj.payload;
        // uint16 STORENUM = pop_u16(&payload);
        // printf("STORENUM= [%d]\n", STORENUM);
    }
    else
    {
        dzlog_error("timeout");
    }
#endif
    }

}
void test_write_normal_data(char * timeNow){
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050011;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum++);
    push_u32(&p, dataTag);
    push_u16(&p, dataTypeID);
    // printf("dataTypeID = %u\n", dataTypeID);
    // printf("dataTag = %u\n", dataTag);
    
    push_nbyte(&p, timeNow, strlen(timeNow));

    // sint32 wbufLen = strlen(wbuf);
    // HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    // push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length  = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    // memset(rsp.payload,0,rsp.length);
#if 0
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK      = pop_u8(&payload);
        printf("ACK= [0x%x]\n", ACK);
    }
    else
    {
        dzlog_error("timeout");
    }
#endif
}

int test_read_normal_data()
{
/*     MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050010;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum ++);
    push_u32(&p, dataTag);
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    dzlog_debug("===>1");
    build_msg(&obj, buf, obj.length, &rsp);
    dzlog_debug("===>2");
    hdzlog_debug(rsp.payload, rsp.length);
    dzlog_debug("===>3");
    xl_send(rsp.payload, rsp.length);
    memset(rsp.payload,0,rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 OPTIONAL = pop_u8(&payload);
        uint8 *data;
        uint32 len = pop_nbyte(&payload, &data);
        printf("OPTIONAL=[0x%x]\n", OPTIONAL);

        memset(rc,0,strlen(rc));

        Hex2Str(data, rc, len);
        printf("BUFLEN=[%d]\n", len);
        printf("BUF= [%s]\n", rc);
        hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    } */
    return 0;
}

void send_test(void)
{
    MSG_OBJ_T obj;
    sint32    recvn;
    sint32    count = 0;
    sint32    i     = 0;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x11111111;

    uint8  payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *p = payloadBuf;

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    char      testsend[200] = {};
    RAW_OBJ_T rsp           = {};
    push_nbyte(&p, (uint8 *)testsend, sizeof(testsend));
    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    for (i = 0; i < 10000; i++)
    {
        if (xl_send(rsp.payload, rsp.length) < 0)
        {
            printf("xlsend fial \n");
            usleep(100000);
        }
        else
        {
            // printf("xlsend send count=%d \n",++count);
        }
    }
}

uint32 G_HANDERNUM;
uint32 G_RECORDNUM;
uint32 G_RECORDNUM_RECV = 0;
void start_read_record_data_test(uint8 did, uint16 inum, uint32 sTime, uint32 eTime,uint8 stype, uint8 mo, uint32 mid, uint32 cid)
{
    sint32 i;
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050012;

    uint8 *p = buf;
    push_u8(&p, did);
    push_u16(&p, inum);

    push_u8(&p, 0);
    if (queryTimeType != 255)
    {
        push_u32(&p, sTime);
        push_u32(&p, eTime);
    }
    push_u8(&p, stype);

    push_u8(&p, mo);  //��ȡ��DIѡ��
    if (mo == 1)
    {
        push_u32(&p, mid);
    }

    // uint32 tmp_minnorDI[200];
    // printf("sortType[%d]\n", sortType);
    // printf("minorDIs[%s]\n", minorDIs);
    // minorDINum = str2intbuf(minorDIs, ",", tmp_minnorDI, 200);
    // printf("minorDINum:%d\n", tmp_minnorDI[0]);
    // if (tmp_minnorDI[0] > 0)
    // {
    //     uint16 diNum = tmp_minnorDI[0];
    //     push_sequnce_length(&p, diNum);
    //     for (i = 0; i < diNum; i++)
    //     {
    //         push_u32(&p, tmp_minnorDI[i + 1]);
    //         printf("tmp_minnorDI[%d] = [%d]\n", i, tmp_minnorDI[i + 1]);
    //     }
    // }
    // else
    // {
    //     push_u8(&p, 0);
    // }
    push_sequnce_length(&p, 1);
    push_u32(&p, cid);

    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint8 *payload   = obj.payload;
        uint32 HANDERNUM = pop_u32(&payload);
        uint32 RECORDNUM = pop_u32(&payload);
        G_HANDERNUM = HANDERNUM;
        G_RECORDNUM = RECORDNUM;
        G_RECORDNUM_RECV = 0;
        printf("HANDERNUM= [%u]\n", HANDERNUM);
        printf("RECORDNUM= [%d]\n", RECORDNUM);
        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}



int step_read_record_test()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050013;
    sint16 i        = 0;

    uint8 *p = buf;
    push_u32(&p, G_HANDERNUM);
    push_u8(&p, 1);
    if (1)
    {
        sint8 checkBuf[100] = {0};
        sint32 checkLen     = strlen(checkInfo);
        HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
        push_nbyte(&p, checkBuf, checkLen >> 1);
    }

    // push_u32(&p, dataBufLen);
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    // hdzlog_debug(rsp.payload, rsp.length);ch
    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        // hdzlog_debug(rsp.payload, recvn);

        pro_parser_msg(rsp.payload, recvn, &obj);
        // printf("msg->prm = 0x%x\n", obj.prm);
        // printf("msg->priority = 0x%x\n", obj.priority);
        // printf("msg->index = 0x%x\n", obj.index);
        // printf("msg->label = 0x%x\n", obj.label);
        // printf("msg->source = %s\n", obj.source);
        // printf("msg->destination = %s\n", obj.destination);
        // printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;

        uint32 TIME_OPTINAL = 0;
        uint32 OPTINAL      = pop_u8(&payload);
        // printf("OPTINAL=[%d]\n", OPTINAL);

        if (OPTINAL == 0)
        {
            return;
        }

        uint32 INFONUM = pop_u16(&payload);
        // printf("INFONUM=[%d]\n", INFONUM);
        uint8 *octet          = NULL;
        sint16 sequnce_length = pop_sequnce_length(&payload);
        // uint32 OCTETSTRLEN = pop_nbyte(&payload, &octet);
        octet = payload;

        // printf("sequnce_length = %d \n", sequnce_length);
        // printf("OCTETSTRLEN = %d\n",OCTETSTRLEN);
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

            // printf("MAJORDI=[%d]\n", MAJORDI);
            // printf("MINORDI=[%d]\n", MINORDI);
            // Hex2Str(data, rc, DATABUFLEN);
            // printf("RAW=[%s]\n", data);
            // printf("DATABUFLEN = %d", DATABUFLEN);
            // printf("BUF=[%s]\n", rc);
            // hdzlog_debug(data,DATABUFLEN);
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

        G_RECORDNUM_RECV++;
    }
    else
    {
        dzlog_error("timeout");
    }
    return 0;
}
void stop_read_record_data_test()
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0x00050014;

    uint8 *p = buf;
    push_u32(&p, handerNum);
    printf("handerNum %u\n", handerNum);
    obj.length  = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK      = pop_u8(&payload);
        printf("ACK=[0x%x]\n", ACK);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void db_read_test(void) 
{
    
    test_read_normal_data();

/*
    getSysTime(&r1);
    printf("startTime [%lu]:[%lu]\n", r1.tv_sec, r1.tv_usec);
    start_read_record_data_test(logicalDeviceNum,infoNum,startTime,endTime,0,majorDIOptinal,majorDI,minorDI);
    getSysTime(&r3);
    printf("start read FinishTime [%lu]:[%lu]\n", r3.tv_sec, r3.tv_usec);
    
    while (1)
    {
        step_read_record_test();
        // sleep(1);
        if(G_RECORDNUM == G_RECORDNUM_RECV)
        {
            printf("G_RECORDNUM_RECV = %d\n", G_RECORDNUM_RECV);
            break;
        }
    }
    printf("end");
    getSysTime(&r2);
    printf("SYNC FinishTime [%lu]:[%lu]\n", r2.tv_sec, r2.tv_usec);

    float m = r2.tv_sec - r1.tv_sec;
    float n = 0;
    if (r2.tv_usec >= r1.tv_usec)
    {
        n = (float)(r2.tv_usec - r1.tv_usec) / 1000000;
    }
    else
    {
        n = (float)(1000000 - r1.tv_usec + r2.tv_usec) / 1000000;
        m--;
    }
    m += n;
    printf("average = %f(s) of[%d]\n", m / G_RECORDNUM_RECV, G_RECORDNUM_RECV);
*/
}

void db_write_test(void)
{
    int i  = 0;
    // struct timeval now;
    // gackcount = 0;

#if 0
    printf("startTime [%lu]:[%lu]\n", now.tv_sec, now.tv_usec);
    for(i = 0; i < gcount;i++)
    {
        getSysTime(&now);
        sprintf(timebuf, "%ld.%ld", now.tv_sec, now.tv_usec);
        // printf("test_write_record_data: %s\n", timebuf);
        test_write_record_data(timebuf);
    }
    if (async == 0)
    {
        getSysTime(&now);
        printf("SYNC FinishTime [%lu]:[%lu]\n", now.tv_sec, now.tv_usec);
    }
#else
    // printf("startTime [%lu]:[%lu]\n", r1.tv_sec, r1.tv_usec);
    if (async == 1)
    {
        for ( i = 0; i < gcount; i++)
        {
            /* code */
        }
        
    }
    else
    {
        getSysTime(&writeTime[0]);
        // sprintf(timebuf, "%ld.%ld", r1.tv_sec, r1.tv_usec);
        // test_write_normal_data(testBuf);
        test_write_record_data(testBuf);
    }
    
/*
    if(async == 1)
    {
        for (i = 0; i < gcount; i++)
        {
            getSysTime(&r1);
            sprintf(timebuf, "%ld.%ld", r1.tv_sec, r1.tv_usec);
            // printf("test_write_record_data: %s\n", timebuf);
            test_write_record_data(timebuf);
        }
    }
    else
    {
        // startindex = 0;
        getSysTime(&r1);
        sprintf(timebuf, "%ld.%ld", r1.tv_sec, r1.tv_usec);
        test_write_record_data(timebuf);
        // startindex++;
    }
*/
#endif

}



void err_msg_test(void)
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = 0xffff1111;

    uint8 payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *p = payloadBuf;
    // MQT_PLUGIN_T mqt = {0};

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};

    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);

    xl_send(rsp.payload, rsp.length);
    recvn = xl_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->prm = 0x%x\n", obj.prm);
        printf("msg->priority = 0x%x\n", obj.priority);
        printf("msg->index = 0x%x\n", obj.index);
        printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload  = obj.payload;
        uint32 tag = pop_u32(&payload);
        sint16 errcode  = pop_u16(&payload);
        printf("errtag = %x\n",tag);
        printf("errcode = %d\n",errcode);
        // hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}


void general_event_test(void)
{
    MSG_OBJ_T obj;
    sint32 recvn;
    obj.prm         = prm;
    obj.priority    = priority;
    obj.index       = _index;
    obj.label       = label;
    obj.source      = source;
    obj.destination = destination;
    obj.tag         = tag;

    uint8 payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *p = payloadBuf;
    // MQT_PLUGIN_T mqt = {0};

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};

    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);

    xl_send(rsp.payload, rsp.length);
}


void test(void)
{
#define MAX_CMD_ARGV 50
    int   argc          = 0;
    char  test_str[100] = "This is a string";
    char *argv[MAX_CMD_ARGV];
    int   n;

    printf("==>1\r\n");
    argc = parse_line(test_str, &argv);
    printf("==>2\r\n");
    for (n = 0; n < argc; n++)
    {
        printf("argv[%d]='%s'\n", n, argv[n]);
    }
}

sint32 task_main(sint32 argc, sint8 **argv)
{
    sint32 i = 0;
    sint32 j = 0;
    sint32 option;

    // test();
    // return 0;

    struct optparse options;
    optparse_init(&options, argv);
    while ((option = optparse_long(&options, longopts, NULL)) != -1)
    {
        switch (option)
        {
            case 'a':
                cmd = options.optarg;
            case 'b':
                port = strtou32(options.optarg);
                break;
            case 'c':
                host = options.optarg;
                break;
            case 'd':
                logicalDeviceNum = strtou32(options.optarg);
                break;
            case 'e':
                infoNum = strtou32(options.optarg);
                break;
            case 'f':
                dataTag = strtou32(options.optarg);
                break;
            case 'g':
                wbuf = options.optarg;
                break;
            case 'h':
                queryTimeType = strtou32(options.optarg);
                break;
            case 'i':
                startTime = strtou32(options.optarg);
                break;
            case 'j':
                endTime = strtou32(options.optarg);
                break;
            case 'k':
                storageTime = strtou32(options.optarg);
                break;
            case 'l':
                sortType = strtou32(options.optarg);
                break;
            case 'm':
                majorDI = strtou32(options.optarg);
                break;
            case 'n':
                minorDINum = strtou32(options.optarg);
                break;
            case 'o':
                minorDI = strtou32(options.optarg);
                break;
            case 'p':
                minorDIs = options.optarg;
                break;
            case 'q':
                handerNum = strtou32(options.optarg);
                break;
            case 'r':
                checkInfo = options.optarg;
                break;
            case 's':
                dataBufLen = strtou32(options.optarg);
                break;
            case 't':
                minorDIOptinal = strtou32(options.optarg);
                break;
            case 'u':
                screenType = strtou32(options.optarg);
                break;
            case 'v':
                storageIndex = strtou32(options.optarg);
                break;
            case 'w':
                dataITDs = options.optarg;
                break;
            case 'x':
                infoNums = options.optarg;
                break;
            case 'y':
                gcount = strtou32(options.optarg);
                break;
            case 'z':
                async = strtou32(options.optarg);
                break;
            case 'A':
                prm = strtou32(options.optarg);
                break;
            case 'B':
                priority = strtou32(options.optarg);
                break;
            case 'C':
                _index = strtou32(options.optarg);
                break;
            case 'D':
                label = strtou32(options.optarg);
                break;
            case 'E':
                source = options.optarg;
            case 'F':
                destination = options.optarg;
                break;
            case 'G':
                tag = strtou32(options.optarg);
                break;
            case 'H':
                timeout = strtou32(options.optarg);
                break;
            case 'I':
                bigpackageTest = strtou32(options.optarg);
                break;

                //
            case 'K':
                subscribeInfo = strtou32(options.optarg);
                break;
            case 'L':
                version = strtou32(options.optarg);
                break;
            case 'M':
                year = strtou32(options.optarg);
                break;

            case 'N':
                month = strtou32(options.optarg);
                break;

            case 'O':
                day = strtou32(options.optarg);
                break;

            case 'P':
                hour = strtou32(options.optarg);
                break;

            case 'Q':
                minute = strtou32(options.optarg);
                break;

            case 'R':
                second = strtou32(options.optarg);
                break;

            case 'S':
                qname = options.optarg;
                break;
                //
            case 'T':
                sub = options.optarg;
                break;
            case 'U':
                url = options.optarg;
                break;
            case 'W':
                majorDIOptinal = strtou32(options.optarg);
                break;
            case 'X':
                dataTypeID = strtou32(options.optarg);
                break;
            case 'V':
                fprintf(stdout, "Vesion      %s\n", VERSION);
                break;
                exit(0);
            case '?':
                fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
                exit(EXIT_FAILURE);
        }
    }

    dzlog_init(NULL, "clientTool");
    // dzlog_info("hello");

    if (bigpackageTest)
    {
        for (i = 0; i < sizeof(bigpackageTestBuf); i++)
        {
            bigpackageTestBuf[i] = bigpackageTest;
        }
        wbuf = bigpackageTestBuf;
    }

    // dzlog_info("hello2");
    // init_socket();

    if (0 == strcmp(cmd, "read_normal_data"))
    {
        read_normal_data();
    }
    else if (0 == strcmp(cmd, "write_normal_data"))
    {
        write_normal_data();
    }
    else if (0 == strcmp(cmd, "start_read_record_data"))
    {
        start_read_record_data();
    }
    else if (0 == strcmp(cmd, "step_read_record"))
    {
        step_read_record();
    }
    else if (0 == strcmp(cmd, "stop_read_record_data"))
    {
        stop_read_record_data();
    }
    else if (0 == strcmp(cmd, "write_record_data"))
    {
        write_record_data();
    }
    else if (0 == strcmp(cmd, "update_record_data"))
    {
        update_record_data();
    }
    else if (0 == strcmp(cmd, "get_storage_num"))
    {
        get_storage_num();
    }  // printf("msg->prm = 0x%x\n", obj.prm);
       // printf("msg->priority = 0x%x\n", obj.priority);
       // printf("msg->index = 0x%x\n", obj.index);
    else if (0 == strcmp(cmd, "get_depth_num"))
    {
        get_depth_num();
    }
    else if (0 == strcmp(cmd, "data_init"))
    {
        data_init();
    }
    else if (0 == strcmp(cmd, "data_init_by_parameter"))
    {
        data_init_by_parameter();
    }
    else if (0 == strcmp(cmd, "factory_Reset"))
    {
        factory_Reset();
    }
    else if (0 == strcmp(cmd, "client_set_subscription"))
    {
        client_set_subscription();
    }
    else if (0 == strcmp(cmd, "client_set_unsubscription"))
    {
        client_set_unsubscription();
    }
    else if (0 == strcmp(cmd, "client_get_subscription"))
    {
        client_get_subscription();
    }
    else if (0 == strcmp(cmd, "start_write_record_data"))  // Niuyq Added
    {
        start_write_record_data();
    }
    else if (0 == strcmp(cmd, "step_write_record"))
    {
        step_write_record();
    }
    else if (0 == strcmp(cmd, "stop_write_record_data"))
    {
        stop_write_record_data();
    }
    else if (0 == strcmp(cmd, "query_app_by_name"))
    {
        query_app_by_name();
    }
    else if (0 == strcmp(cmd, "query_app"))
    {
        query_app();
    }
    else if (0 == strcmp(cmd, "unregister_app"))
    {
        unregister_app();
    }
    else if (0 == strcmp(cmd, "app_register"))
    {
        app_register();
    }
    else if (0 == strcmp(cmd, "reboot"))
    {
        reboot();
    }
    else if (0 == strcmp(cmd, "get_power_state"))
    {
        get_power_state();
    }
    else if (0 == strcmp(cmd, "get_ethernet_state"))
    {
        get_ethernet_state();
    }
    else if (0 == strcmp(cmd, "send_test"))
    {
        send_test();
    }
    else if (0 == strcmp(cmd, "db_write_test"))
    {
        db_write_test();
    }
    else if (0 == strcmp(cmd, "db_read_test"))
    {
        db_read_test();
    }
    else if (0 == strcmp(cmd, "err_msg_test"))
    {
        err_msg_test();
    }
    else if (0 == strcmp(cmd, "general_event_test"))
    {
        general_event_test();
    }
    else
    {
        fprintf(stderr, "CMD not support %s\n", cmd);
    }

    // dzlog_debug("msg sendStart [%lu]:[%lu]", sendStart.tv_sec,
    //             sendStart.tv_usec);
    // dzlog_debug("msg sendDone [%lu]:[%lu]", sendDone.tv_sec,
    // sendDone.tv_usec); dzlog_debug("msg recvDone [%lu]:[%lu]",
    // recvDone.tv_sec, recvDone.tv_usec);

    // dzlog_debug("msg sendDone - sendStart [%lu]:[%lu]",
    //             sendDone.tv_sec - sendStart.tv_sec,
    //             sendDone.tv_usec - sendStart.tv_usec);

    // dzlog_debug("msg recvDone - sendDone [%lu]:[%lu]",
    //             recvDone.tv_sec - sendDone.tv_sec,
    //             recvDone.tv_usec - sendDone.tv_usec);

    return 0;
}

void init(char *url)
{
    int    ret = -1;
    sint32 type;
    UNUSED(type);

    if (*url == 'm')
    {
        type = 0;
    }
    else if (*url == 'u')
    {
        type = 1;
    }
    else if (*url == 't')
    {
        type = 2;
    }

    // mqtt
    if (type == 0)
    {
        MQTT_TOPIC_T subtopic[1] = {0};
        ret                      = xl_channel_create(url, &s_ctx);
        if (ret != 0)
        {
            fprintf(stdout, "xl_channel_create fail");
            exit(-1);
        }

        if (0 != xl_channel_set_mqtt_user_ID(s_ctx, source))
        {
            fprintf(stderr, "xl_channel_set_mqtt_user_ID fail");
            exit(-1);
        }

        subtopic[0].qos = 0;
        snprintf(subtopic[0].topic, sizeof(subtopic[0].topic), "+/%s",source);
        if (0 != xl_channel_set_mqtt_sub_topic(s_ctx, 1, &subtopic))
        {
            fprintf(stderr, "xl_channel_set_mqtt_sub_topic fail");
            exit(-1);
        }

        if (0 != xl_channel_set_mqtt_default_topic(s_ctx, chdes))
        {
            fprintf(stderr, "xl_channel_set_mqtt_user_ID fail");
            exit(-1);
        }
    }
    // udp
    else if (type == 1)
    {
        ret = xl_channel_create(url, &s_ctx);
        if (ret != 0)
        {
            fprintf(stdout, "xl_channel_create fail");
            exit(-1);
        }
    }
    // tcp
    else if (type == 2)
    {
        ret = xl_channel_create(url, &s_ctx);
        if (ret != 0)
        {
            fprintf(stdout, "xl_channel_create fail");
            exit(-1);
        }
    }

    if (0 != xl_channel_init(s_ctx))
    {
        fprintf(stderr, "xl_channel_init fail");
        exit(-1);
    }
}

void mqtt_connect_handler_task(void)
{
    s_mqtt_status = xl_channel_get_connect_status(s_ctx);

    switch (s_mqtt_status)
    {
        case INIT:
            printf("xl_channel_connect\n");
            xl_channel_connect(s_ctx);
            sleep(1);
            break;
        case CONNECT_FAIL:
            xl_channel_disconnect(s_ctx);
            xl_channel_connect(s_ctx);
            break;
        default:
            break;
    }
}

void heart_bear(uint16 mid)
{
    MSG_OBJ_T obj = {0};
    RAW_OBJ_T req = {0};
    // printf("receive the app heart beat message\n");
    obj.prm         = 0;
    obj.priority    = 0;
    obj.index       = mid;
    obj.label       = 0;
    obj.source      = (sint8 *)source;
    obj.destination = "M-smiOS";//(sint8 *)chdes;
    obj.tag         = 0x00000010;
    uint32 value    = 12;
    uint8  payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *p = payloadBuf;
    push_u32(&p, value);
    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &req);
    xl_channel_write(s_ctx, (const char *)req.payload, req.length);
    printf("\n[event] send heart beart ack to smios\n");
}

void login_to_smiOS(void)
{
    MSG_OBJ_T obj;
    sint32    i;
    obj.prm         = 1;
    obj.priority    = 1;
    obj.index       = 1;
    obj.label       = 1;
    obj.source      = (char *)source;
    obj.destination = (char *)"smiOS";
    obj.tag         = 0x00010010;

    uint8        payloadBuf[MAX_PAYLOAD_LEN];
    uint8 *      p   = payloadBuf;
    MQT_PLUGIN_T mqt = {0};

    UNUSED(p);
    obj.length  = 0;
    obj.payload = payloadBuf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    push_u32(&p, 1111);
    strcpy((sint8 *)mqt.comName, (char *)source);

    uint32 tmp_subInfo[1024];
    uint32 n = hexStr2U32buf(sub, ",", tmp_subInfo, 1024);

    mqt.subr.subscribeInfoNum = n;
    for (i = 0; i<n> APP_MAX_SUB_NUM ? APP_MAX_SUB_NUM : n; i++)
    {
        mqt.subr.subscribeInfo[i] = tmp_subInfo[i];
    }

    mqt.version         = 200;
    mqt.dataTime.year   = 2019;
    mqt.dataTime.month  = 11;
    mqt.dataTime.day    = 23;
    mqt.dataTime.hour   = 10;
    mqt.dataTime.minute = 8;
    mqt.dataTime.second = 22;

    push_mqt_plugin(&p, &mqt);
    obj.length = p - payloadBuf;
    build_msg(&obj, payloadBuf, obj.length, &rsp);
    xl_channel_write(s_ctx, (const char *)rsp.payload, rsp.length);
}

void recv_task(void)
{
    sint32      recvn         = 0;
    sint32      putn          = 0;
    sint32      msg_len       = 0;
    sint32      offset        = 0;
    sint32      surplus_len   = 0;
    RAW_OBJ_T   rsp           = {0};
    MSG_OBJ_T   obj           = {0};
    static char surplus[MAX_PAYLOAD_LEN] = {0};
    static int  surplus_num   = 0;

    // do
    // {

    if (surplus_num == 4096)
    {
        // dzlog_debug("surplus_num :%d", surplus_num);
    }

    if (surplus_num > 0)
    {
        memcpy(rsp.payload, surplus, surplus_num);
        // dzlog_debug("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        // hdzlog_debug(surplus, surplus_num);

        recvn = xl_channel_read(s_ctx, (sint8 *)rsp.payload + surplus_num,
                                MAX_PAYLOAD_LEN - surplus_num);
        if(recvn >= 0)
        {
            // dzlog_debug(
            //     "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
            // hdzlog_debug(rsp.payload + surplus_num, recvn);

            recvn += surplus_num;
            surplus_num = 0;
        }

    }
    else
    {
        recvn = xl_channel_read(s_ctx, (sint8 *)rsp.payload, MAX_PAYLOAD_LEN);
        if(recvn > 0)
        {
            // dzlog_debug("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
            // hdzlog_debug(rsp.payload, recvn);
        }

    }

    if (recvn > 0)
    {
        // printf("recvn:%d\n", recvn);
        gettimeofday(&recvDone, NULL);

        offset = 0;
        while (1)
        {
            if (recvn <= 0)
            {
                // printf("====>1\n");
                break;
            }

            msg_len = pro_parser_msg(rsp.payload + offset, recvn, &obj);
            if (msg_len < 0)
            {
                memcpy(surplus, rsp.payload + offset, recvn);
                surplus_num = recvn;
                // dzlog_debug("recvn = %d offset= %d", recvn,offset);
                // hdzlog_debug(rsp.payload + offset, recvn);
                // printf("====>2 surplus_num=%d\n", surplus_num);
                break;
            }
            else
            {
                putn = recvn;
                recvn -= msg_len;
                offset += msg_len;
            }

#if 0
            printf("=======>read\n");
            printf("@@@@ msg->tag = 0x%x\n", obj.tag);
            printf("@@@@ msg->prm = 0x%x\n", obj.prm);
            printf("@@@@ msg->priority = 0x%x\n", obj.priority);
            printf("@@@@ msg->index = 0x%x\n", obj.index);
            printf("@@@@ msg->label = 0x%x\n", obj.label);
            printf("@@@@ msg->source = %s\n", obj.source);
            printf("@@@@ msg->destination = %s\n", obj.destination);

#endif

            // if (obj.tag == 0x00010010)
            // {
            //     uint8 *payload = obj.payload;
            //     uint8  ACK     = pop_u8(&payload);
            //     // printf("ACK= [0x%x]\n", ACK);

            //     g_flag_login = LOGIN_STATE_DONE;
            //     printf("\n[event] %s LOGIN_STATE_DONE\n", source);
            // }

            switch (obj.tag)
            {
                case 0x00000010:
                    heart_bear(obj.index);
                    break;
                case 0x00000001:
                    printf("\n[event] %s EMERGENCY_BLACKOUT\n", source);
                    break;
                case 0x00000002:
                    printf("\n[event] %s SYSTEM_BLACKOUT\n", source);
                    break;
                case 0x00000003:
                    printf("\n[event] %s SYSTEM_RESET\n", source);
                    break;
                case 0x00000004:
                    printf("\n[event] %s SYSTEM_POWER_ON\n", source);

                    break;
                case 0x00000005:
                    printf("\n[event] %s SYSTEM_CLOCK_CHECK\n", source);

                    break;
                case 0x00050001:
                {
                    uint8 *payload = obj.payload;
                    uint8 *tmp_xdr = payload;
                    uint8  did     = pop_u8(&tmp_xdr);
                    uint16 infoNum = pop_u16(&tmp_xdr);
                    uint32 dataTag = pop_u32(&tmp_xdr);

                    // printf("\n[event] %s DB_UPDATE"
                        //    "[%u]:[%u]:[0x%08x]\n",
                        //    source, did, infoNum, dataTag);
                }
                break;
                case 0x00050002:
                {
                    uint8 *payload = obj.payload;
                    uint8 *tmp_xdr = payload;
                    uint8  type    = pop_u8(&tmp_xdr);
                    uint8  did     = pop_u8(&tmp_xdr);
                    uint8 dataClassIDOption = 0;
                    uint8 dataClassID = 0;
                    if(type)
                    {
                        dataClassIDOption = pop_u8(&tmp_xdr);
                        if (dataClassIDOption)
                        {
                            dataClassID = pop_u8(&tmp_xdr);
                        }
                    }

                    printf("\n[event] %s DB_INIT type:[%u] did:[%u] dataClassIDOption:[%u] dataClassID:[%u]\n", source,
                           type, did, dataClassIDOption, dataClassID);
                }
                break;
                case 0x00050011:
                {
                    if(async){}
                    else
                    {
                        gackcount++;
                        getSysTime(&writeTime[gackcount]);
                        printf("gackcount = %d \n",gackcount);
                        if (gackcount == gcount)
                        {
                            fileWrite();
                        }
                        else
                        {
                            test_write_normal_data(testBuf);
                        }
                    }                    
                }
                break;
                case 0x00050015:
                {
                    if(async){}
                    else
                    {
                        gackcount++;
                        getSysTime(&writeTime[gackcount]);
                        printf("gackcount = %d \n",gackcount);
    // dzlog_debug("msg sendStart [%lu]:[%lu]", sendStart.tv_sec,
    //             sendStart.tv_usec);
    // dzlog_debug("msg sendDone [%lu]:[%lu]", sendDone.tv_sec, sendDone.tv_usec);
    // dzlog_debug("msg recvDone [%lu]:[%lu]", recvDone.tv_sec, recvDone.tv_usec);
                        if (gackcount == gcount)
                        {
                            fileWrite();
                        }
                        else
                        {
                            test_write_record_data(testBuf);
                        }
                    }                    
                    /* if(async)
                    {
                        gackcount++;
                        if (gackcount == gcount)
                        {
                            // static struct timeval finTime;
                            getSysTime(&r2);
                            printf("ASYNC FinishTime [%lu]:[%lu]\n", r2.tv_sec,
                                   r2.tv_usec);

                            float m = r2.tv_sec - r1.tv_sec;
                            float n = 0;
                            if (r2.tv_usec >= r1.tv_usec)
                            {
                                n = (float)(r2.tv_usec - r1.tv_usec) / 1000000;
                            }
                            else
                            {
                                n = (float)(1000000 - r1.tv_usec + r2.tv_usec) / 1000000;
                                m--;
                            }
                            m += n;
                            printf("average = %f(s) of[%d]\n", m / gcount, gcount);
                        }
                    }
                    else
                    {
                        // if (putn > 0)
                        // {
                        //     memcpy(tmpbuf, rsp.payload, putn);
                        //     tmpbuf_len  = putn;
                        //     flag_tmpbuf = 1;
                        // }
                        gackcount++;
                        if (gackcount == gcount)
                        {
                            // static struct timeval finTime;
                            getSysTime(&r2);
                            printf("SYNC FinishTime [%lu]:[%lu]\n",
                                   r2.tv_sec, r2.tv_usec);

                            float m = r2.tv_sec - r1.tv_sec;
                            float n = 0;
                            if(r2.tv_usec >= r1.tv_usec )
                            {
                                n = (float)(r2.tv_usec - r1.tv_usec)/1000000;
                            }
                            else
                            {
                                n = (float)(1000000 - r1.tv_usec + r2.tv_usec) / 1000000;
                                m--;
                            }
                            m += n;
                            printf("average = %f(s) of[%d]\n", m/gcount, gcount);
                        }
                        else
                        {
                            char timebuf[128] = {0};
                            struct timeval now;
                            getSysTime(&now);
                            sprintf(timebuf, "%ld.%ld", now.tv_sec,
                                    now.tv_usec);
                            test_write_record_data(timebuf);
                        }
                    } */
                }
                break;
                case 0x11111111:
                {
                    static int            start = 0;
                    static int            count = 0;
                    static struct timeval start_time;
                    static struct timeval end_time;
                    if (start == 0)
                    {
                        start = 1;
                        count = 0;
                        getSysTime(&start_time);
                    }
                    count++;
                    printf("count = %d\n", count);
                    if (count == 10000)
                    {
                        getSysTime(&end_time);
                        printf("start time [%lu]:[%lu]\n", start_time.tv_sec,
                               start_time.tv_usec);
                        printf("end time [%lu]:[%lu]\n", end_time.tv_sec,
                               end_time.tv_usec);
                        start = 0;
                    }
                }
                break;

                default:

                    // printf("put recvn:%d\n",recvn);
                    if (putn > 0)
                    {
                        memcpy(tmpbuf, rsp.payload, putn);
                        tmpbuf_len  = putn;
                        flag_tmpbuf = 1;
                    }

                    break;
            }
        }
    }
    // } while (recvn > 0);
}

void login_task(void)
{
    static struct timeval lastLoginTs;
    struct timeval        now;
    switch (g_flag_login)
    {
        case LOGIN_STATE_INIT:
            login_to_smiOS();
            gettimeofday(&lastLoginTs, NULL);
            g_flag_login = LOGIN_STATE_WAIT;
            break;
        case LOGIN_STATE_WAIT:
            gettimeofday(&now, NULL);

            if (now.tv_sec - lastLoginTs.tv_sec > 5)
            {
                g_flag_login = LOGIN_STATE_INIT;
            }
            break;
        case LOGIN_STATE_DONE:
            break;
        default:
            break;
    }
}

void loop(void)
{
    for (;;)
    {
        mqtt_connect_handler_task();
        if (s_mqtt_status == CONNECT_SUCC)
        {
            // login_task();
            recv_task();
        }
        if (s_mqtt_status == CONNECT_SUCC && g_flag_login == LOGIN_STATE_DONE)
        {
            // test_write_dbCenter_task();
        }
        usleep(100);
    }
}

void console_task(void *p)
{
    int   argc = 0;
    char *argv[1024];
    int   n;

    for (;;)
    {
        gets(cmdbuf);
        argc = parse_line(cmdbuf, &argv);
        // for (n = 0; n < argc; n++)
        // {
        //     printf("argv[%d]='%s'\n", n, argv[n]);
        // }

        if (argc > 0)
        {
            task_main(argc, argv);
        }

        printf("\n> ");
    }
}

sint32 main(sint32 argc, sint8 **argv)
{
    sint32    i = 0;
    sint32    j = 0;
    sint32    option;
    pthread_t tid;

    fp = fopen("db.csv","at+");
    if(fp == NULL)
    {
        printf("file open error!\n");
        return -1;
    }
    else
    {
        printf("file open OK!\n");
    }
     fprintf(fp,"write num,write ms \n");
    fclose(fp);
    fp = NULL;

    bufWrite();


    struct optparse options;
    optparse_init(&options, argv);
    while ((option = optparse_long(&options, longopts, NULL)) != -1)
    {
        switch (option)
        {
            case 'E':
                source = options.optarg;
                break;
            case 'F':
                destination = options.optarg;
                break;
            case 'J':
                chdes = options.optarg;
                break;
            case 'U':
                url = options.optarg;
                break;
            case 'V':
                fprintf(stdout, "Vesion      %s\n", VERSION);
                break;
                exit(0);
            case '?':
                fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
                exit(EXIT_FAILURE);
        }
    }

    printf("url:%s\n", url);
    printf("destination:%s\n", destination);
    printf("source:%s\n", source);
    printf("chdes:%s\n", chdes);
    init(url);
    pthread_create(&tid, NULL, console_task, NULL);
    loop();

    return 0;
}
