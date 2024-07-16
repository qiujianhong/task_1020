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
#include <time.h>
#include <sys/stat.h>
#include "optparse.h"
#include "xdrCustom.h"
#include "msgService.h"
#include "zlog.h"
#include "hex2byte.h"
#include "clientTool.h"
#include "baseType.h"
#include "msgHelper.h"

#define VERSION "0.1"

sint32 iSocketClient;
struct sockaddr_in tSocketServerAddr;

struct timeval sendStart;
struct timeval sendDone;
struct timeval recvDone;


sint8 *cmd;
sint8 *host;
sint8 *checkInfo;
sint8 *wbuf;
sint8 *infoNums;
sint8 *minorDIs;
sint8 *dataITDs;
uint16 port;
uint8 logicalDeviceNum;
uint16 infoNum;
uint32 dataTag;
uint16 dataTypeID;
uint32 queryTimeType;
uint32 startTime;
uint32 endTime;
uint32 storageTime;
uint32 sortType;
uint32 majorDI;
uint32 minorDINum;
uint32 minorDI;

uint32 handerNum;
uint32 dataBufLen;
uint32 majorDIOptinal=1;
uint32 screenType;
uint32 storageIndex;
uint32 choice;

uint8 prm = 0;
uint8 priority = 0;
uint16 _index = 0;
uint16 label = 0;
sint8 *source = "clientTool";
sint8 *destination = "dbCenter";
uint32 tag;
uint32 timeout = 10;
uint32 bigpackageTest = 0;

uint8 bigpackageTestBuf[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER)*2] = {0};
sint8 rc[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER) * 3] = {};
uint8 buf[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER) * 3];
sint8 writebuf[(MAX_PAYLOAD_LEN - MAX_MSG_HEADER) * 3];

struct optparse_long longopts[] =
    {

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
        {"majorDIOptinal", 't', OPTPARSE_REQUIRED},
        
        {"screenType", 'u', OPTPARSE_REQUIRED},
        {"storageIndex", 'v', OPTPARSE_REQUIRED},
        {"dataITDs", 'w', OPTPARSE_REQUIRED},
        {"infoNums", 'x', OPTPARSE_REQUIRED},
        {"prm", 'A', OPTPARSE_REQUIRED},
        {"priority", 'B', OPTPARSE_REQUIRED},
        {"index", 'C', OPTPARSE_REQUIRED},
        {"label", 'D', OPTPARSE_REQUIRED},
        {"source", 'E', OPTPARSE_REQUIRED},
        {"destination", 'F', OPTPARSE_REQUIRED},
        {"tag", 'G', OPTPARSE_REQUIRED},
        {"timeout", 'H', OPTPARSE_REQUIRED},
        {"SV", 'V', OPTPARSE_NONE},
        {"big",'I',OPTPARSE_REQUIRED},
        {"dataTypeID", 'J', OPTPARSE_REQUIRED},
        {"choice", 'K', OPTPARSE_REQUIRED},
        {0}};

uint32 strtou32(char *str) 
{
     uint32 temp=0;
    // uint32 fact=1;
    // unsigned char len=strlen(str);  
    // unsigned char i;
    // for(i=len;i>0;i--)
    // {
    //     temp+=((str[i-1]-0x30)*fact);
    //     fact*=10;
    // }
    temp = strtoul(str, NULL,0);
    printf("str:%s \n",str);
    return temp;
}

#if 0
static sint32 build_msg(MSG_OBJ_T *msgObj, sint8 *payload, sint32 payload_len, RAW_OBJ_T *rsp)
{
    uint8 *p = rsp->payload;
    p[MSG_PRM_OFFSET] = (msgObj->prm & 0x01) | (msgObj->priority << 1);
    p[MSG_INDEX_OFFSET] = msgObj->index >> 8;
    p[MSG_INDEX_OFFSET + 1] = msgObj->index & 0xff;
    p[MSG_LABEL_OFFSET] = msgObj->label >> 8;
    p[MSG_LABEL_OFFSET + 1] = msgObj->label & 0xff;
    sint8 *source_addr = p + MSG_SOURCE_OFFSET;
    uint8 source_len = strlen(msgObj->source);
    strncpy(source_addr, msgObj->source, source_len);
    sint8 *des_addr = source_addr + source_len + 1;
    uint8 des_len = strlen(msgObj->destination);
    strncpy(des_addr, msgObj->destination, des_len);
    uint8 *tag_addr = des_addr + des_len + 1;
    tag_addr[3] = msgObj->tag & 0xff;
    tag_addr[2] = (msgObj->tag >> 8) & 0xff;
    tag_addr[1] = (msgObj->tag >> 16) & 0xff;
    tag_addr[0] = (msgObj->tag >> 24) & 0xff;
    uint8 *len_add = tag_addr + 4;
    uint8 *xdr_payload_addr;

    if (payload_len < XDR_SIGNEL_LEN)
    {
        len_add[0] = payload_len;
        xdr_payload_addr = len_add + MSG_PYLOAD_OFFSET_1;
    }
    else
    {
        len_add[0] = 0x82;
        len_add[1] = (payload_len >> 8) & 0xff;
        len_add[2] = payload_len & 0xff;
        xdr_payload_addr = len_add + MSG_PYLOAD_OFFSET_3;
    }
    memcpy(xdr_payload_addr, payload, payload_len);
    rsp->length = xdr_payload_addr - p + payload_len;
    return rsp->length;
}



static sint32 pro_parser_msg(sint8 *rxbuf, sint32 rxbuf_len, MSG_OBJ_T *msgObj)
{
    msgObj->prm = GET_PRM(rxbuf);
    msgObj->priority = GET_PRIORITY(rxbuf);
    msgObj->index = GET_INDEX(rxbuf);
    msgObj->label = GET_LABEL(rxbuf);
    msgObj->source = GET_SOURCE(rxbuf);
    msgObj->destination = GET_DESTINATION(rxbuf);
    sint8 *tag_addr = GET_TAG_ADDR(msgObj->destination);
    msgObj->tag = GET_TAG(tag_addr);
    sint8 *len_addr = GET_LENGTH_ADDR(tag_addr);
    msgObj->length = GET_LENGTH(len_addr);
    msgObj->payload = len_addr + (msgObj->length < 0x80 ? MSG_PYLOAD_OFFSET_1 : MSG_PYLOAD_OFFSET_3);
}
#endif

void read_normal_data()
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
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum);
    push_u8(&p, 1);
    push_u32(&p, dataTag);
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    dzlog_debug("===>1");
    build_msg(&obj, buf, obj.length, &rsp);
    dzlog_debug("===>2");
    hdzlog_debug(rsp.payload, rsp.length);
    dzlog_debug("===>3");
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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

        Hex2Str(data, rc, len);
        printf("BUFLEN=[%d]\n", len);
        printf("BUF= [%s]\n", rc);
        //hdzlog_debug(data, len);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050011;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum);
    push_u8(&p, 1);
    push_u32(&p, dataTag);
    push_u16(&p, dataTypeID);
    printf("dataTypeID = %u\n", dataTypeID);
    printf("dataTag = %u\n", dataTag);
    sint32 wbufLen = strlen(wbuf);
    HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint8 ACK = pop_u8(&payload);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050012;

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

    push_u8(&p, majorDIOptinal);   //获取次DI选项
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
            push_u32(&p, tmp_minnorDI[i+1]);
            printf("tmp_minnorDI[%d] = [%u]\n", i, tmp_minnorDI[i+1]);
        }
    }
    else
    {
        push_u8(&p, 0);
    }

    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint32 HANDERNUM = pop_u32(&payload);
        uint32 RECORDNUM = pop_u32(&payload);
        printf("HANDERNUM= [%u]\n", HANDERNUM);
        printf("RECORDNUM= [%d]\n", RECORDNUM);
        //hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

#if 0
void step_read_record_old()
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

    uint8 *p = buf;
    push_u32(&p, handerNum);

    sint8 checkBuf[100] = {0};
    sint32 checkLen = strlen(checkInfo);
    HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
    push_fixbyte(&p, checkBuf, 6);
    // push_nbyte(&p, checkInfo, 6);
    push_u32(&p, dataBufLen);
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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

        uint32 STARTTIME = pop_u32(&payload);
        uint32 ENDTIME = pop_u32(&payload);
        uint32 STORAGETIME = pop_u32(&payload);
        uint32 INFONUM = pop_u16(&payload);
        uint8 *octet = NULL;
        uint32 OCTETSTRLEN = pop_nbyte(&payload, &octet);

        uint32 MAJORDI = pop_u32(&octet);
        uint32 MINORDIOPTINAL = pop_u8(&octet);
        uint32 MINORDI = 0;
        if (MINORDIOPTINAL)
        {
            MINORDI = pop_u32(&octet);
        }
        uint32 DATABUFLEN = pop_u32(&octet);

        uint8 *data;
        // printf("DATABUFLEN = %d",DATABUFLEN);
        pop_fixbyte(&octet, &data,DATABUFLEN);
        // hdzlog_debug(data,DATABUFLEN);
        Hex2Str(data, rc, DATABUFLEN);

        printf("OPTINAL=[%d]\n", OPTINAL);
        if(OPTINAL){
            printf("STARTTIME=[%d]\n", STARTTIME);
            printf("ENDTIME=[%d]\n", ENDTIME);
            printf("STORAGETIME=[%d]\n", STORAGETIME);
            printf("INFONUM=[%d]\n", INFONUM);
            printf("OCTETSTRLEN=[%d]\n", OCTETSTRLEN);

            printf("MAJORDI=[%u]\n", MAJORDI);
            printf("MINORDIOPTINAL=[%d]\n", MINORDIOPTINAL);
            printf("MINORDI=[%u]\n", MINORDI);
            printf("DATABUFLEN=[%d]\n", DATABUFLEN);
            printf("BUF=[%s]\n", rc);
        }

    }
    else
    {
        dzlog_error("timeout");
    }
}
#endif

void step_read_record()
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
    sint16 i = 0;

    uint8 *p = buf;
    push_u32(&p, handerNum);
    push_u8(&p, majorDIOptinal);
    if (majorDIOptinal)
    {
        sint8 checkBuf[100] = {0};
        sint32 checkLen = strlen(checkInfo);
        HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
        dzlog_debug("chechInfo:%s",checkInfo);
        push_nbyte(&p, checkBuf, checkLen>>1);
        hdzlog_debug(checkBuf, checkLen >> 1);
    }

    //push_u32(&p, dataBufLen);
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint32 OPTINAL = pop_u8(&payload);
        printf("OPTINAL=[%d]\n", OPTINAL);

        if(OPTINAL == 0)
        {
            return;
        }


        uint32 INFONUM = pop_u16(&payload);
        printf("INFONUM=[%d]\n", INFONUM);
        uint8 *octet = NULL;
        sint16 sequnce_length = pop_sequnce_length(&payload);
        //uint32 OCTETSTRLEN = pop_nbyte(&payload, &octet);
        octet =payload;

 

        printf("sequnce_length = %d \n",sequnce_length);
        printf("=========================\n");
        for(i=0; i < sequnce_length; i++)
        {
            uint32 MAJORDI = 0;
            uint32 MAJORDIOPTINAL = 0;
            memset(rc,0,sizeof(rc));
            
            MAJORDIOPTINAL = pop_u8(&octet);
            if(MAJORDIOPTINAL)
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
            //uint32 DATABUFLEN = pop_u32(&octet);
            uint8 *data = 0;
            uint32 DATABUFLEN = pop_nbyte(&octet, &data);
            //pop_fixbyte(&octet, &data,DATABUFLEN); 
            Hex2Str(data, rc, DATABUFLEN);
            printf("DATABUFLEN = %d\n",DATABUFLEN);
            printf("BUF=[%s]\n", rc);
            //hdzlog_debug(data,DATABUFLEN);
        }

        TIME_OPTINAL = pop_u8(&octet);
        if (TIME_OPTINAL)
        {
            uint32 STARTTIME   = pop_u32(&octet);
            uint32 ENDTIME     = pop_u32(&octet);
            uint32 STORAGETIME = pop_u32(&octet);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050014;

    uint8 *p = buf;
    push_u32(&p, handerNum);
    printf("handerNum %u\n",handerNum);
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint8 ACK = pop_u8(&payload);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050015;

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
    sint32 checkLen = strlen(checkInfo);
    HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
    push_nbyte(&p, checkBuf, checkLen >>1);
    //push_fixbyte(&p, checkBuf, 6);

    // push_nbyte(&p, checkInfo, 6);
    sint32 wbufLen = strlen(wbuf);
    HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050016;

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
    sint32 checkLen = strlen(checkInfo);
    HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
     push_nbyte(&p, checkBuf, checkLen >>1);
    //push_fixbyte(&p, checkBuf, 6);
    
    sint32 wbufLen = strlen(wbuf);
    HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint8 ACK = pop_u8(&payload);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050020;

    uint8 *p = buf;
    //追加
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

    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint32 STORAGENUM = pop_u32(&payload);

        printf("STORAGENUM=[%d]\n", STORAGENUM);

        //hdzlog_debug(data, len);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050021;

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
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint32 DEPTHNUMM = pop_u32(&payload);

        printf("DEPTHNUMM=[%d]\n", DEPTHNUMM);

        //hdzlog_debug(data, len);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050030;

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
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint8 ACK = pop_u8(&payload);

        printf("ACK=[%d]\n", ACK);

        //hdzlog_debug(data, len);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050031;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum);
    push_u8(&p, choice);
    if(choice == 0)
    {
        push_u16(&p, dataTypeID);
    }
    else if(choice ==1)
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


    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint8 ACK = pop_u8(&payload);

        printf("ACK=[%d]\n", ACK);

        //hdzlog_debug(data, len);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050032;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);

    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint8 ACK = pop_u8(&payload);

        printf("ACK=[%d]\n", ACK);

        //hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

void client_set_subscription()
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
    obj.tag = 0x00050040;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    uint16 tmp_infoNum[1024];
    printf("infoNums = %s\n", infoNums);
    printf("dataITDs = %s\n", dataITDs);
    uint32 n = str2shortbuf(infoNums, ",", tmp_infoNum, 1024);

    if (n > 0)
    {
        push_u8(&p, n);
        for (i = 0; i < n; i++)
        {
            printf("push_u16: %d\n", tmp_infoNum[i]);
            push_u16(&p, tmp_infoNum[i]);
        }
    }
    else
    {
        push_u8(&p, 0);
    }

    uint32 tmp_dataITDs[200];
    uint32 idn = str2intbuf(dataITDs, ",", tmp_dataITDs, 200);

    if (idn > 0)
    {
        push_u8(&p, idn);
        for (i = 0; i < idn; i++)
        {
            printf("push_u32: %d\n", tmp_dataITDs[i]);
            push_u32(&p, tmp_dataITDs[i]);
        }
    }
    else
    {
        push_u8(&p, 0);
    }

    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint8 ACK = pop_u32(&payload);

        printf("ACK=[%d]\n", ACK);

        //hdzlog_debug(data, len);
    }
    else
    {
        dzlog_error("timeout");
    }
}

sint32 init_socket()
{
    struct timeval tv_out;
    tv_out.tv_sec = timeout;
    tv_out.tv_usec = 0;

    iSocketClient = socket(AF_INET, SOCK_DGRAM, 0);
    if(iSocketClient <0)
    {
        printf("create socket fail!\n");
    }
    tSocketServerAddr.sin_family = AF_INET;
    tSocketServerAddr.sin_port = htons(port); /* host to net, sint16 */

    memset(tSocketServerAddr.sin_zero, 0, 8);
    inet_aton(host, &tSocketServerAddr.sin_addr);
    setsockopt(iSocketClient, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
}

sint32 udp_send(void *buf, sint32 buflen)
{
    gettimeofday(&sendStart,NULL);
    socklen_t AddrLen = sizeof(struct sockaddr);
    sendto(iSocketClient, buf, buflen, 0,
           (const struct sockaddr *)&tSocketServerAddr, AddrLen);
    gettimeofday(&sendDone, NULL);
}

sint32 udp_recv(void *buf, sint32 bufMaxLen)
{
    socklen_t AddrLen = sizeof(struct sockaddr);
    sint32 n = recvfrom(iSocketClient, buf, bufMaxLen, 0, (struct sockaddr *)&tSocketServerAddr, &AddrLen);
    if (n > 0)
    {
        gettimeofday(&recvDone, NULL);
        hdzlog_debug(buf, n);
    }
    return n;
}

void start_write_record_data()
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
    obj.tag = 0x00050042;
    uint8 *p = buf;

    obj.length = p - buf;
    obj.payload = buf;
    //dzlog_error("test");
    hdzlog_debug(buf, 6);
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint32 HANDERNUM = pop_u32(&payload);
        //uint32 RECORDNUM = pop_u32(&payload);
        printf("HANDERNUM= [%u]\n", HANDERNUM);
        //printf("RECORDNUM= [%d]\n", RECORDNUM);
        //hdzlog_debug(data, len);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050043;

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
    sint32 checkLen = strlen(checkInfo);
    HexStrToByte(checkInfo, checkBuf, checkLen & 0xfffffffe);
    //push_fixbyte(&p, checkBuf, 6);
    push_nbyte(&p, checkInfo, checkLen>>1);
    //
    sint32 wbufLen = strlen(wbuf);
    HexStrToByte(wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);

    printf("majorDI = %u \n", majorDI);
    printf("minorDI = %u \n", minorDI);
    printf("infoNum = %d\n", infoNum);
    printf("startTime = %d\n", startTime);
    printf("endTime = %d \n", endTime);
    printf("storageTime = %d\n", storageTime);
   printf("check data:");
    for(i = 0 ; i < checkLen; i++)
    {
        printf("data= %02x: ", checkBuf[i]); 
    }
    printf("\n");
   printf("bin data:");
    for(i = 0 ; i < wbufLen; i++)
    {
        printf("data = %02x:", writebuf[i]); 
    }
    printf("\n"); 

   recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
    obj.prm = prm;
    obj.priority = priority;
    obj.index = _index;
    obj.label = label;
    obj.source = source;
    obj.destination = destination;
    obj.tag = 0x00050044;

    uint8 *p = buf;
    push_u32(&p, handerNum);
    printf("handerNum %u\n",handerNum);
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
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
        uint8 ACK = pop_u8(&payload);
        printf("ACK=[0x%x]\n", ACK);
    }
    else
    {
        dzlog_error("timeout");
    }
}

sint32 main(sint32 argc, sint8 **argv)
{

    sint32 i = 0;
    sint32 j = 0;
    sint32 option;
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
            majorDIOptinal = strtou32(options.optarg);
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
        case 'J':
            dataTypeID = strtou32(options.optarg);
            break;
        case 'K':
            choice     = strtou32(options.optarg);
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
    dzlog_info("hello");

    if(bigpackageTest)
    {
        for(i = 0; i<sizeof(bigpackageTestBuf); i++ )
        {
            bigpackageTestBuf[i] = bigpackageTest;
        }
        wbuf = bigpackageTestBuf;
    }

    dzlog_info("hello2");
    init_socket();

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
    }        // printf("msg->prm = 0x%x\n", obj.prm);
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
    else if (0 == strcmp(cmd, "set_subscription"))
    {
        client_set_subscription();
    }
    else if (0 == strcmp(cmd, "start_write_record_data"))  //Niuyq Added
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
    else
    {
        fprintf(stderr, "CMD not support %s\n", cmd);
    }

    dzlog_debug("msg sendStart [%lu]:[%lu]", sendStart.tv_sec,
                sendStart.tv_usec);
    dzlog_debug("msg sendDone [%lu]:[%lu]", sendDone.tv_sec, sendDone.tv_usec);
    dzlog_debug("msg recvDone [%lu]:[%lu]", recvDone.tv_sec, recvDone.tv_usec);

    dzlog_debug("msg sendDone - sendStart [%lu]:[%lu]",
                sendDone.tv_sec - sendStart.tv_sec,
                sendDone.tv_usec - sendStart.tv_usec);

    dzlog_debug("msg recvDone - sendDone [%lu]:[%lu]",
                recvDone.tv_sec - sendDone.tv_sec,
                recvDone.tv_usec - sendDone.tv_usec);

    return 0;
}

sint32 t_start_write_record_data()
{
    sint32 i;
    MSG_OBJ_T obj = {0};
    sint32 recvn;
    
    obj.source = "clientTool";
    obj.destination = "dbCenter";
    obj.tag = 0x00050042;
    uint8 *p = buf;

    obj.length = p - buf;
    obj.payload = buf;
    //dzlog_error("test");
    //hdzlog_debug(buf, 6);
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint32 HANDERNUM = pop_u32(&payload);
        //uint32 RECORDNUM = pop_u32(&payload);
        printf("HANDERNUM= [%u]\n", HANDERNUM);

        return HANDERNUM;
    }
    else
    {
        printf("timeout");
    }
    return -1;
}

 sint32 t_step_write_record(sint32 uhanderNum, 
 sint32 ilogicalDeviceNum,
 sint32 imajorDI,
sint8 cmajorDIOptinal,
 sint32 iminorDI,
 sint32 istartTime,
 sint32 iendTime,
 sint32 istorageTime,
 short sinfoNum
)
{
    int i = 0;
    MSG_OBJ_T obj={0};
    sint32 recvn;
    unsigned char _check_info[6] = "010203040507";
    unsigned char _wbuf[20] =  "b00102030405060708090A0B0C0D0E0F";
     
    // unsigned char _check_info[6] = {0x01,0x06,0x02,0x03,0x04,0x05};
    // unsigned char _wbuf[20] =  {0x01,0xb6,0xc2,0xd3,0xe4,0xf5,0xa1,0xb6,0xc2,0xd3,0xe4,0xf5,0xa1,0xb6,0xc2,0xd3,0xe4,0x15};
    // obj.prm = prm;
    // obj.priority = priority;
    // obj.index = _index;
    // obj.label = label;
    obj.source = "clientTool";
    obj.destination = "dbCenter";
    obj.tag = 0x00050043;

    uint8 *p = buf;
///
    push_u32(&p, uhanderNum);

    push_u8(&p, ilogicalDeviceNum);
    push_u32(&p, imajorDI);
    if (cmajorDIOptinal)
    {
        push_u8(&p, 1);
        push_u32(&p, iminorDI);
    }
    else
    {
        push_u8(&p, 0);
    }

    push_u32(&p, istartTime);
    push_u32(&p, iendTime);
    push_u32(&p, istorageTime);
    push_u16(&p, sinfoNum);
   /////
    sint8 checkBuf[100] = {0};

    sint32 checkLen = strlen(_check_info);
    HexStrToByte(_check_info, checkBuf, checkLen & 0xfffffffe);
    //push_fixbyte(&p, checkBuf, 6);
    push_nbyte(&p, checkInfo, checkLen>>1);
    //
    sint32 wbufLen = strlen(_wbuf);
    HexStrToByte(_wbuf, writebuf, wbufLen & 0xfffffffe);
    push_nbyte(&p, writebuf, wbufLen >> 1);
    obj.length = p - buf;
    obj.payload = buf;

    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);

    printf("majorDI = %u \n", imajorDI);
    printf("minorDI = %u \n", iminorDI);
    printf("infoNum = %d\n", sinfoNum);
    printf("startTime = %d\n", istartTime);
    printf("endTime = %d \n", iendTime);
    printf("storageTime = %d\n", istorageTime);
   printf("check data:");
    for(i = 0 ; i < checkLen; i++)
    {
        printf("data= %02x: ", checkBuf[i]); 
    }
    printf("\n");
   printf("bin data:");
    for(i = 0 ; i < wbufLen; i++)
    {
        printf("data = %02x:", writebuf[i]); 
    }
    printf("\n"); 

   recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
   if (recvn > 0)
   {
        pro_parser_msg(rsp.payload, recvn, &obj);
        // printf("msg->prm = 0x%x\n", obj.prm);
        // printf("msg->priority = 0x%x\n", obj.priority);
        // printf("msg->index = 0x%x\n", obj.index);
        //printf("msg->label = 0x%x\n", obj.label);
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

void t_stop_write_record_data( sint32 uhanderNum)
{
    MSG_OBJ_T obj={0};
    sint32 recvn;
    // obj.prm = prm;
    // obj.priority = priority;
    // obj.index = _index;
    // obj.label = label;
    obj.source = "clientTool";
    obj.destination = "dbCenter";
    obj.tag = 0x00050044;

    uint8 *p = buf;
    push_u32(&p, uhanderNum);
    printf("handerNum %u\n",uhanderNum);
    obj.length = p - buf;
    obj.payload = buf;
    // sint8 rxbuf[1400];
    RAW_OBJ_T rsp = {};
    build_msg(&obj, buf, obj.length, &rsp);
    //hdzlog_debug(rsp.payload, rsp.length);
    udp_send(rsp.payload, rsp.length);
    recvn = udp_recv(rsp.payload, MAX_PAYLOAD_LEN);
    if (recvn > 0)
    {
        pro_parser_msg(rsp.payload, recvn, &obj);
        // printf("msg->prm = 0x%x\n", obj.prm);
        // printf("msg->priority = 0x%x\n", obj.priority);
        // printf("msg->index = 0x%x\n", obj.index);
        // printf("msg->label = 0x%x\n", obj.label);
        printf("msg->source = %s\n", obj.source);
        printf("msg->destination = %s\n", obj.destination);
        printf("msg->tag = 0x%x\n", obj.tag);
        uint8 *payload = obj.payload;
        uint8 ACK = pop_u8(&payload);
        printf("ACK=[0x%x]\n", ACK);
    }
    else
    {
        dzlog_error("timeout");
    }
}
sint32 main1(sint32 argc, sint8 **argv)
{
    int i;
    sint32 handleNo;
    sint32 ilogicalDeviceNum = 0;
    sint32 imajorDI = 9;
    sint8 cmajorDIOptinal =1;
    sint32 iminorDI =4;
    sint32 istartTime =6;
    sint32 iendTime =8;
    sint32 istorageTime =9;
    short sinfoNum =10;
    
    dzlog_init(NULL, "clientTool");
    dzlog_info("hello");

    //host = "10.12.42.210";
    //port = 8598;

    port = 5001;
    host = "127.0.0.1";
    init_socket();

    handleNo = t_start_write_record_data();

    for(i = 0; i < 2000; i++)
    {
        iminorDI += 1;
        t_step_write_record( handleNo, ilogicalDeviceNum, imajorDI,
                cmajorDIOptinal,
                iminorDI,
                istartTime,
                iendTime,
                istorageTime,
                sinfoNum);
    }

    t_stop_write_record_data(handleNo);


}
