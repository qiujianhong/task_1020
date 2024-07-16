#ifdef AREA_HUNAN

#include "CtsAll.h"
#include "area.h"
#include "iniparser.h"
#include "CtsPlc.h"
#include "power.h"

//#include "mapmsg_split.h"
//地址为00表示没接 端口RS485_PORT1
uint8 gTemperatureAddr ;   //温湿度传感器地址
uint8 gTemperaturePort;    //温湿度传感器端口

uint8 gSmokeAddr;   //烟雾传感器地址
uint8 gSmokePort;  //端口

uint8 gWaterAddr;   //水浸传感器地址
uint8 gWaterPort;   //端口

uint8 gTransTempAddr[6];    //变压器桩头温度传感器地址
uint8 gTransTempPort;   //端口

uint8 gEventTemperatureSeq[PLC_NUM]; //断路器环境温度越限事件
uint8 gEventProtectSeq[PLC_NUM]; //保护动作事件序号
uint8 gEventSwitchChangeSeq[PLC_NUM];    //闸位变化事件序号

OOP_OAD_U gTemperatureOad = {0x60E10200};
OOP_OAD_U gSmokeOad =       {0x60E20200};
OOP_OAD_U gWaterOad =       {0x60E30200};
OOP_OAD_U gTransTempOad =   {0x60E40200};
extern uint8 gRouterStudy;

//属性99（HPLC模块存储曲线标记）∷=unsigned F2096300
//开启标志：bit0：HPLC模块存储曲线标记（0：关闭；1：开启）
uint8 gHPLCModuleCurveFlag; 

//属性 101 HPLC-CCO 模块曲线存储数据项 F2096500
OOP_MODULE_CURVE_STORE_DATA_T gModuleCurveStoreData;

#define MAX_STA_CURVE_STATE_GET_NUM    30

uint8 gCurveStoreSyncFlag[PLC_NUM]={0};

//模块曲线默认曲线配置方案
//698.45单相表数据项配置
const OOP_TO_DATA_ID_PARAM_T gDataIdParam_single_oop[]=
{
    //数据标识                     
    {{0x20000200},  0x20000200,      5},     //电压
    {{0x20010200},  0x20010200,      7},     //电流
    {{0x20010400},  0x20010400,      7},     //零线电流
    {{0x20040200},  0x20040200,      7},     //有功功率
    {{0x00000201},  0x00000201,      7},     //组合有功电能
    {{0x00100201},  0x00100201,      7},     //正向有功电能
    {{0x00200201},  0x00200201,      7},     //反向有功电能
};
const uint8 gDataIdParam_single_oop_len = sizeof(gDataIdParam_single_oop)/sizeof(gDataIdParam_single_oop[0]);

//698.45三相表数据项配置
const OOP_TO_DATA_ID_PARAM_T gDataIdParam_three_oop[]=
{
    //数据标识                     
    {{0x20000200},    0x20000200,      11},     //电压
    {{0x20010200},    0x20010200,      17},     //电流
    {{0x20010400},    0x20010400,      7},     //零线电流
    {{0x20040200},    0x20040200,      22},     //有功功率
    {{0x20050200},    0x20050200,      22},     //无功功率
    {{0x200A0200},    0x200A0200,      14},     //功率因数
    {{0x00000201},    0x00000201,      7},     //组合有功电能
    {{0x00100201},    0x00100201,      7},     //正向有功电能
    {{0x00200201},    0x00200201,      7},     //反向有功电能
    {{0x00300201},    0x00300201,      7},     //组合无功1电能
    {{0x00400201},    0x00400201,      7},     //组合无功2电能
    {{0x00500201},    0x00500201,      7},     //第一象限无功电能总
    {{0x00600201},    0x00600201,      7},     //第二象限无功电能总
    {{0x00700201},    0x00700201,      7},     //第三象限无功电能总
    {{0x00800201},    0x00800201,      7},     //第四象限无功电能总
    {{0x10100201},    0x10100201,      7},     //正向有功最大需量及发生时间(总)
    {{0x10200201},    0x10200201,      7},     //反向有功最大需量及发生时间(总)
};
const uint8 gDataIdParam_three_oop_len = sizeof(gDataIdParam_three_oop)/sizeof(gDataIdParam_three_oop[0]);

//645单相表数据项配置 
const OOP_TO_DATA_ID_PARAM_T gDataIdParam_single_645[]=
{
    //数据标识                     
    {{0x20000201},    0x02010100,      2},     //电压
    {{0x20010201},    0x02020100,      3},     //电流
    {{0x20010400},    0x02800001,      3},     //零线电流
    {{0x20040201},    0x02030000,      3},     //瞬时有功
    {{0x00000201},    0x00000000,      4},     //组合有功电能
    {{0x00100201},    0x00010000,      4},     //正向有功电能
    {{0x00200201},    0x00020000,      4},     //反向有功电能
};
const uint8 gDataIdParam_single_645_len = sizeof(gDataIdParam_single_645)/sizeof(gDataIdParam_single_645[0]);

//645三相表数据项配置
const OOP_TO_DATA_ID_PARAM_T gDataIdParam_three_645[]=
{
    //数据标识                     
    {{0x20000200},        0x0201FF00,      6},     //电压
    {{0x20010200},        0x0202FF00,      9},     //电流
    {{0x20010400},        0x02800001,      3},     //零线电流
    {{0x20040200},        0x0203FF00,      12},     //有功功率
    {{0x20050200},        0x0204FF00,      12},     //无功功率
    {{0x00000201},        0x00000000,      4},     //组合有功电能
    {{0x00100201},        0x00010000,      4},     //正向有功电能
    {{0x00200201},        0x00020000,      4},     //反向有功电能
    {{0x00300201},        0x00030000,      4},     //组合无功1电能
    {{0x00400201},        0x00040000,      4},     //组合无功2电能
    {{0x00500201},        0x00050000,      4},     //第一象限无功电能总
    {{0x00600201},        0x00060000,      4},     //第二象限无功电能总
    {{0x00700201},        0x00070000,      4},     //第三象限无功电能总
    {{0x00800201},        0x00080000,      4},     //第四象限无功电能总
    {{0x10100201},        0x01010000,      8},     //正向有功最大需量及发生时间(总)
    {{0x10200201},        0x01020000,      8},     //反向有功最大需量及发生时间(总)
};
const uint8 gDataIdParam_three_645_len = sizeof(gDataIdParam_three_645)/sizeof(gDataIdParam_three_645[0]);



SENSOR_SEND_ATTR_T g_SensorSendAttr[SENSOR_MAX] = 
{/* prtl,                   code,   dataAddr,   dataNum,    di */
    {SENSOR_PRTL_MODBUS,    0x03,   0x0000,     0x0002,     0xFFFFFFFF},
    {SENSOR_PRTL_MODBUS,    0x03,   0x0000,     0x0006,     0xFFFFFFFF},
    {SENSOR_PRTL_MODBUS,    0x01,   0x0000,     0x0001,     0xFFFFFFFF},
    {SENSOR_PRTL_645,       0xFF,   0xFFFF,     0xFFFF,     0x08400001},
};

SENSOR_RECV_ATTR_T g_SensorRecvAttr[SENSOR_MAX] = 
{/* prtl,                   code,   dataLen,    di              fillFunc,           saveFunc */
    {SENSOR_PRTL_MODBUS,    0x03,   0x04,       0xFFFFFFFF,     FillHAndTResult,    SaveHAndTDB},
    {SENSOR_PRTL_MODBUS,    0x03,   0x0C,       0xFFFFFFFF,     FillWaterResult,    SaveWaterDB},
    {SENSOR_PRTL_MODBUS,    0x01,   0x01,       0xFFFFFFFF,     FillSmokeResult,    SaveSmokeDB},
    {SENSOR_PRTL_645,       0xFF,   0xFF,       0x08400001,     Fill645TResult,     Save645TDB},
};

// 该函数内部已执行CRC字节的高低字节换序，该函数的返回值是已经调换的CRC。
const unsigned char auchCRCHi[] = 
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,  
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,  
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,  
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,  
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,  
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,  
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,  
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,  
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,  
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,  
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,  
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,  
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,  
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,  
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40  
} ;  

const char auchCRCLo[] = 
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,  
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,  
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,  
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,  
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,  
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,  
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,  
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,  
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,  
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,  
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,  
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,  
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,  
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,  
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,  
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,  
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,  
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,  
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,  
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,  
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,  
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,  
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,  
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,  
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,  
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40  
};

/**********************************************************************
* @name      : CRC16
* @brief     ：求CRC16校验码
* @param[in] ：uint8*  pMsg       输入缓冲区
               uint16  len        帧长度
* @param[out]：
* @return    ：校验值
* @Create    : 
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
uint16 CRC16(uint8 *pMsg, uint16 len)
{
    uint8 uchCRCHi =0xff;   /*CRC高字节初始化*/
    uint8 uchCRCLo =0xff;   /*CRC低字节初始化*/
    uint8 uIndex ;          /*CRC循环索引*/

    while (len--)   /*传输消息缓冲区*/
    {
        uIndex = uchCRCHi ^ *pMsg++;    /*计算CRC*/
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
        uchCRCLo = auchCRCLo[uIndex];
    }
    return (uchCRCHi << 8 | uchCRCLo);
}

#define REQ_CTRL_CODE 0x11
uint32 PackSensorPkt(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_SEND_FRAME_U *frame, uint16 *frameLen)
{
    SENSOR_SEND_ATTR_T *attr = &g_SensorSendAttr[dev->type];
    SENSOR_SEND_FRAME_MODBUS_T *modbusFrame = NULL;
    MeterPrtlFmt_T meterData = {0};

    if (attr->prtl == SENSOR_PRTL_MODBUS)
    {
        modbusFrame = &frame->modbusFrame;
        modbusFrame->pdu.addr = dev->addr.modbusAddr;
        modbusFrame->pdu.code = attr->code;
        modbusFrame->pdu.dataAddr = htons(attr->dataAddr);
        modbusFrame->pdu.dataNum = htons(attr->dataNum);
        modbusFrame->checkCode = htons(CRC16((uint8 *)&modbusFrame->pdu, sizeof(modbusFrame->pdu)));
        *frameLen = sizeof(SENSOR_SEND_FRAME_MODBUS_T);
    }
    else if (attr->prtl == SENSOR_PRTL_645)
    {
        memcpy(meterData.MtrAddr, dev->addr.mtrAddr, sizeof(dev->addr.mtrAddr));
        meterData.CtrlCode = REQ_CTRL_CODE;
        meterData.DataLen = sizeof(attr->di);
        memcpy(meterData.DataBuff, (uint8 *)&attr->di, meterData.DataLen);
        *frameLen = taskmng_meter_prtl_645_pack(frame->buf, meterData);
    }
    else
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG, "PackSensorPkt unsupported protocol.\n");
        return 1;
    }

    return 0;
}

uint32 SensorSend(uint8 pipe, RS485_RS232_PARA *commPara, SENSOR_SEND_FRAME_U *frame, uint16 frameLen)
{
    int ret;
    TASK_FMT_DEBUG(pipe, COMM_LOG, "SensorSend\n");
    TASK_BUF_DEBUG(pipe, COMM_LOG, (uint8 *)frame, frameLen);
    ret = rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,6,PARITY_EVEN, (uint8 *)frame, frameLen);
    //ret = rs485_RS232_send(commPara, (uint8 *)frame, frameLen);
    TASK_FMT_DEBUG(pipe, COMM_LOG, "rs485_RS232_send\n");    
    if (ret != 0)
    {
        TASK_FMT_TRACE(pipe, COMM_LOG, "发送失败 ret is %d\n",ret);
        return 1;
    }

    return 0;
}

uint32 SendSensorPkt(uint8 pipe, SENSOR_DEV_INFO_T *dev, RS485_RS232_PARA *commPara)
{
    SENSOR_SEND_FRAME_U frame = {{{0}}};
    uint16 frameLen = 0;

    if (PackSensorPkt(pipe, dev, &frame, &frameLen) != 0)
    {
        return 1;
    }

    if (SensorSend(pipe, commPara, &frame, frameLen) != 0)
    {
        return 1;
    }
    dev->needMtrFlag = 1;
    return 0;
}

void SensorWriteNormalData(uint8 pipe, DB_CLIENT DBClientHandle, uint8 *dataBuf,uint16 dataLen, OOP_OAD_U oad)
{
    int ret;
    NOMAL_OAD_T nomalOad;
    nomalOad.logicId = 0;
    nomalOad.oad = oad;
    nomalOad.classtag = 3;
    nomalOad.infoNum = 0;
    //TASK_FMT_TRACE( -1, RELAY_LOG,"写普通数据到数据中心oad %08x infonum %d len%d \n",Oad.value,NomalOad.infoNum,DataLen);

    ret = db_write_nomal(DBClientHandle, &nomalOad, dataBuf, dataLen);
    TASK_FMT_DEBUG(pipe, COMM_LOG, "db_write_nomal,oad=0x%08x\n",oad.value);
    if (ret != 0)
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG,"普通数据写入数据库失败 ret %d oad %08x\n",ret, oad.value);
        TASK_FMT_TRACE(pipe, RELAY_LOG,"普通数据写入数据库失败 ret %d oad %08x\n",ret, oad.value);
    }
}

extern uint8 LocalAddr[6];    //终端地址
void SensorWriteRecordData(uint8 pipe, DB_CLIENT DBClientHandle, uint8 *dataBuf, uint16 dataLen, OOP_OAD_U oad)
{
    int     ret;
    WRITE_RECORD_T inData = {{0}};
    uint32  storeNo = 0;
    time_t  now = time(NULL);

    inData.recordOAD.optional = 0;
    inData.recordOAD.logicId = 0;
    inData.recordOAD.infoNum = 0; /* 0代表终端 */
    inData.recordOAD.road.oadCols.oad[0] = oad;
    inData.recordOAD.classtag = 5;
    
    inData.MAC.nNum = 0;
//    memcpy_r(inData.MAC.value, LocalAddr, 6);

    inData.colStartTm = now;
    inData.colStoreTm = now;
    inData.colEndTm = now;

    inData.inDatalen = dataLen;
    memcpy(inData.inDataBuf, dataBuf, dataLen);

    frame_printf(inData.inDataBuf, dataLen);

    ret = db_write_record(DBClientHandle, &inData, &storeNo);
    TASK_FMT_DEBUG(pipe, COMM_LOG, "db_write_record,oad=0x%08x\n",oad.value);
    if (ret != 0)
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG, "写失败 ret is %d\n",ret);
        TASK_FMT_TRACE (pipe, REC_LOG, "写失败 ret is %d\n",ret);
    }
}

uint32 SaveHAndTDB(uint8 pipe, SENSOR_DEV_INFO_T *dev, DB_CLIENT DBClientHandle)
{
    OOP_OAD_U temOad = {0x26000200};
    OOP_OAD_U humOad = {0x26010200};
    OOP_LONG3V_T tem;
    OOP_WORD3V_T hum;

    memset((uint8*)&tem, 0xff, sizeof(tem));
    tem.nNum = 3;
    tem.rsv = 0;
    set_bit_value((uint8*)&tem.rsv, 32, 0);
    tem.nValue[0] = dev->result.hAndTResult.tem;
    SensorWriteNormalData(pipe, DBClientHandle, (uint8*)&tem, sizeof(tem), temOad);

    memset((uint8*)&hum, 0xff, sizeof(hum));
    hum.nNum = 3;
    hum.rsv = 0;
    set_bit_value((uint8*)&hum.rsv, 32, 0);
    hum.nValue[0] = dev->result.hAndTResult.hum;
    SensorWriteNormalData(pipe, DBClientHandle, (uint8*)&hum, sizeof(hum), humOad);

    return 0;
}

uint32 SaveWaterDB(uint8 pipe, SENSOR_DEV_INFO_T *dev, DB_CLIENT DBClientHandle)
{
    OOP_OAD_U waterOad = {0x2B020200};
    uint8 water = dev->result.u8Result;
    SensorWriteNormalData(pipe, DBClientHandle, (uint8*)&water, sizeof(water), waterOad);

    return 0;
}

uint32 SaveSmokeDB(uint8 pipe, SENSOR_DEV_INFO_T *dev, DB_CLIENT DBClientHandle)
{
    OOP_OAD_U smokeOad = {0x2B010200};
    uint8 smoke = dev->result.u8Result;
    SensorWriteNormalData(pipe, DBClientHandle, (uint8*)&smoke, sizeof(smoke), smokeOad);

    return 0;
}

uint32 Save645TDB(uint8 pipe, SENSOR_DEV_INFO_T *dev, DB_CLIENT DBClientHandle)
{
    OOP_OAD_U temOad = {0x2A030200};
    OOP_LONG3V_T tem;

    memset((uint8*)&tem, 0xff, sizeof(tem));
    tem.nNum = 3;
    tem.rsv = 0;
    set_bit_value((uint8*)&tem.rsv, 32, 0);
    tem.nValue[0] = dev->result.a645TResult[0];
    set_bit_value((uint8*)&tem.rsv, 32, 1);
    tem.nValue[1] = dev->result.a645TResult[1];
    set_bit_value((uint8*)&tem.rsv, 32, 2);
    tem.nValue[2] = dev->result.a645TResult[2];
    SensorWriteNormalData(pipe, DBClientHandle, (uint8*)&tem, sizeof(tem), temOad);

    return 0;
}

/* 调用存数据中心接口 */
uint32 SaveDB(uint8 pipe, SENSOR_DEV_INFO_T *dev, DB_CLIENT DBClientHandle)
{
    SENSOR_RECV_ATTR_T *attr = NULL;

    if (dev->type >= SENSOR_MAX)
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG, "type >= SENSOR_MAX,type=%u\n", dev->type);
        return 1;
    }

    attr = &g_SensorRecvAttr[dev->type];

    if (dev->needWriteFlag == 1)
    {
        attr->saveFunc(pipe, dev, DBClientHandle);
        dev->needWriteFlag = 0;
        dev->needMtrFlag = 0;
    }

    return 0;
}
/* 参数合法性由调用者保证 */
uint32 FillHAndTResult(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu)
{
    dev->result.hAndTResult.hum = htons(*((uint16 *)&pdu->modbusPdu.data[0]));
    dev->result.hAndTResult.tem = (int16)htons(*((uint16 *)&pdu->modbusPdu.data[0 + sizeof(uint16)]));
    dev->needWriteFlag = 1;

    return 0;
}
/* 参数合法性由调用者保证 */
uint32 FillWaterResult(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu)
{
    /* 水浸报警 */
    if (pdu->modbusPdu.data[2] == 0x03 && pdu->modbusPdu.data[3] == 0xE8)
    {
        dev->result.u8Result = 1;
    }
    else
    {
        dev->result.u8Result = 0;
    }

    dev->needWriteFlag = 1;

    return 0;
}
/* 参数合法性由调用者保证 */
uint32 FillSmokeResult(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu)
{
    /* 烟感报警 */
    if (pdu->modbusPdu.data[0] == 0x01)
    {
        dev->result.u8Result = 1;
    }
    else
    {
        dev->result.u8Result = 0;
    }

    dev->needWriteFlag = 1;

    return 0;
}

#define RSP_CTRL_CODE 0x91
/* 参数合法性由调用者保证 */
uint32 Fill645TResult(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu)
{
    uint8 addr[6] = {0};
    uint8 ctrlCode = 0;
    uint16 dataLen = 0;
    uint8 dataBuf[256] = {0};
    uint32 i = 0;
    uint8 tmpBuf[30];
    int8 ret = 0;

    ret = taskmng_645_meter_data_unpack(sizeof(pdu->buf), pdu->buf, addr, &ctrlCode, &dataLen, dataBuf);
    //TASK_FMT_DEBUG(pipe, COMM_LOG, "Fill645TResult,dataLen=%d\n", dataLen);
    //TASK_BUF_DEBUG(pipe, COMM_LOG, (uint8 *)dataBuf, dataLen);

    if (ret == CHECK_RESULT_FAILUER)
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG, "Fill645TResult,CHECK_RESULT_FAILUER\n");
        return 1;
    }

    if (g_SensorRecvAttr[dev->type].di != *(uint32 *)&dataBuf[0] || ctrlCode != RSP_CTRL_CODE)
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG, "Fill645TResult err,dev->di=0x%08x,di=0x%08x,ctrlCode=%u\n",g_SensorRecvAttr[dev->type].di, *(uint32 *)&dataBuf[0], ctrlCode);
        return 1;
    }

    if (dataLen >= 10)
    {
        while (i < 3)
        {
            memset(tmpBuf, 0x00, 30);
            Dlt645_ComBcdToBin(dataBuf + 4 + i * 2, 2, tmpBuf, 2, TRUE); /* 4为DI长度 */
            memcpy((uint8 *)&dev->result.a645TResult[i], tmpBuf,2);
            i++;
        }
    }
    else
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG, "Fill645TResult err,dataLen=%u\n",dataLen);
        return 1;
    }

    dev->needWriteFlag = 1;

    return 0;
}

uint32 UnpackSensorPkt(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu)
{
    SENSOR_RECV_ATTR_T *attr = NULL;

    if (dev->type >= SENSOR_MAX)
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG, "type >= SENSOR_MAX,type=%u\n", dev->type);
        return 1;
    }

    attr = &g_SensorRecvAttr[dev->type];

    if (attr->prtl == SENSOR_PRTL_MODBUS)
    {
        if (pdu->modbusPdu.code != attr->code || pdu->modbusPdu.dataLen != attr->dataLen)
        {
            TASK_FMT_DEBUG(pipe, COMM_LOG, "type=%u,code=%u,dataLen=%u\n", dev->type, pdu->modbusPdu.code, pdu->modbusPdu.dataLen);
            return 1;
        }
    }

    return attr->fillFunc(pipe, dev, pdu);
}

uint32 FillDevResult(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu, DB_CLIENT DBClientHandle)
{
    if (UnpackSensorPkt(pipe, dev, pdu) != 0)
    {
        return 1;
    }
    TASK_FMT_DEBUG(pipe, COMM_LOG, "UnpackSensorPkt\n");
    if (SaveDB(pipe, dev, DBClientHandle) != 0)
    {
        return 1;
    }
    TASK_FMT_DEBUG(pipe, COMM_LOG, "SaveDB\n");
    return 0;
}

uint32 SensorRecv(uint8 pipe, RS485_RS232_PARA *commPara, SENSOR_RECV_PDU_U *pdu)
{
    int recvLen = 0;
    memset(pdu, 0, sizeof(SENSOR_RECV_PDU_U));
    recvLen = rs485_RS232_recv(pipe, (uint8 *)pdu, sizeof(SENSOR_RECV_PDU_U));

    if (recvLen <= 0)
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG, "recvLen err,recvLen=%d\n", recvLen);
        return 1;
    }
    TASK_FMT_DEBUG(pipe, COMM_LOG, "SensorRecv,recvLen=%d\n", recvLen);
    TASK_BUF_DEBUG(pipe, COMM_LOG, (uint8 *)pdu, recvLen);
    return 0;
}

uint32 RecvSensorPkt(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu, RS485_RS232_PARA *commPara)
{
    SENSOR_RECV_ATTR_T *attr = NULL;

    if (SensorRecv(pipe, commPara, pdu) != 0)
    {
        return 1;
    }

    if (dev->type >= SENSOR_MAX)
    {
        TASK_FMT_DEBUG(pipe, COMM_LOG, "type >= SENSOR_MAX,type=%u\n", dev->type);
        return 1;
    }

    attr = &g_SensorRecvAttr[dev->type];

    TASK_FMT_DEBUG(pipe, COMM_LOG, "RecvSensorPkt\n");
    if (attr->prtl == SENSOR_PRTL_MODBUS)
    {
        if (dev->addr.modbusAddr != pdu->modbusPdu.addr)
        {
            TASK_FMT_DEBUG(pipe, COMM_LOG, "dev->addr=%u,pdu->modbusPdu.addr=%u\n", dev->addr.modbusAddr, pdu->modbusPdu.addr);
            return 1;
        }

        TASK_FMT_DEBUG(pipe, COMM_LOG, "addr check ok\n");
    }

    return 0;
}

#define MAX_ATTEMPT 100
#define MTR_AGAIN_CNT 1
uint32 LoRaRecvProc(LORA_AMR_T *loRaAmr, uint32 devIndex)
{
    //uint32 i = 0;
    uint32 mtrAgainCnt = MTR_AGAIN_CNT;
    //uint32 devIndex = SENSOR_DEV_MAX_NUM;
    SENSOR_RECV_PDU_U pdu = {{0}};
    SENSOR_DEV_INFO_T *dev = &loRaAmr->dev[devIndex];

    while (1)
    {
        //for (i = 0; i < MAX_ATTEMPT; i++)
        //{
        usleep(10000); /* 睡10ms */

        if (RecvSensorPkt(-1, dev, &pdu, &loRaAmr->commPara) == 0)
        {
            if (FillDevResult(-1, dev, &pdu, loRaAmr->DBClientHandle) == 0)
            {
                break;
            }
        }
        //}

        if (mtrAgainCnt == 0)
        {
            break;
        }
        /* 传感器出现抄读失败，进行重抄 */
        if (dev->needMtrFlag == 1)
        {
            SendSensorPkt(-1, dev, &loRaAmr->commPara);
            mtrAgainCnt--;
        }
        else
        {
            break;
        }
    }
    
    return 0;
}

uint32 LoRaSendProc(LORA_AMR_T *loRaAmr, uint32 devIndex)
{
    SENSOR_DEV_INFO_T *dev = &loRaAmr->dev[devIndex];

    return SendSensorPkt(-1, dev, &loRaAmr->commPara);
}

#define SENSOR_MTR_INTERVAL 60 /* 抄表间隔，单位：秒 */
time_t g_SensorTimeCnt[AMR_THREAD_NUM] = {0}; /* 过抄读时间点计数 */
uint32 SensorMtrTimeCheck(uint8 pipe)
{
    time_t *cnt = &g_SensorTimeCnt[pipe];
    time_t now;
    time(&now);
    /* 第一次进来直接抄，不等待 */
    if (*cnt == 0)
    {
        *cnt = now / SENSOR_MTR_INTERVAL;
        return 0;
    }

    if (now / SENSOR_MTR_INTERVAL != *cnt)
    {
        *cnt = now / SENSOR_MTR_INTERVAL;
        return 0;
    }

    return 1;
}

uint32 LoRaMtrProc(LORA_AMR_T *loRaAmr)
{
    uint32 i = 0;

    for (i = 0; i < loRaAmr->devNum; i++)
    {
        LoRaSendProc(loRaAmr, i);
        LoRaRecvProc(loRaAmr, i);
        //CalcCurIndex();
    }

    return 0;
}

uint32 ReadLoRaConf(LORA_AMR_T *loRaAmr)
{
    //uint32 i = 0;
    loRaAmr->devNum = 3;

    loRaAmr->dev[0].addr.modbusAddr = 0x01;
    loRaAmr->dev[0].type = SENSOR_H_AND_T;

    loRaAmr->dev[1].addr.modbusAddr = 0x02;
    loRaAmr->dev[1].type = SENSOR_WATER;

    loRaAmr->dev[2].addr.modbusAddr = 0x03;
    loRaAmr->dev[2].type = SENSOR_SMOKE;

    return 0;
}



//SENSOR_AMR_T g_SensorAmr;
int rs485_RS232_Init_Pro(uint8 pipe, RS485_RS232_PARA *pCommPara, uint16 speed, PARITY_TYPE_E parity)
{
    UART_DEVICE_T *dev;
    int fd = -1;
    int ret = 0;
    //printf("初始化%s,speed is %d\n",pCommPara->deviceName,speed);
    //TASK_FMT_DEBUG( -1, COMM_LOG, "第一次获取设备初始化 pCommPara->dev11 is %p\n",pCommPara->dev);
    if (pCommPara->dev == NULL)
    {
        pCommPara->dev = (UART_DEVICE_T *)hal_device_get(pCommPara->deviceName);
        TASK_FMT_DEBUG(pipe, COMM_LOG, "第一次获取设备初始化 pCommPara->dev is %p\n",pCommPara->dev);
    }

    if (NULL == pCommPara->dev)
    {
        return -1;    
    }

    dev = pCommPara->dev;
    // 打开设备
    if (speed == 0)
    {
        speed = 2400;
    }
    pCommPara->Buad= speed;
    ret = dev->uart_param_set(dev, speed, 8, 1, parity);
    pCommPara->pDev = dev; 
    fd =  dev->uart_get_handle(dev);
    TASK_FMT_DEBUG(pipe, COMM_LOG, "初始化 dev is %p,speed is %d,parity is %u\n", dev, speed, parity);
    TASK_FMT_DEBUG(pipe, COMM_LOG, "初始化 fd is %d,ret is %d\n", fd, ret);
    return ret;
}

uint32 ReadSensorConf(SENSOR_AMR_T *sensorAmr)
{
    uint32 i = 0;
    sensorAmr->devNum = 0;

    if (gWaterAddr != 0)
    {
        sensorAmr->dev[i].addr.modbusAddr = gWaterAddr;
        sensorAmr->dev[i].type = SENSOR_WATER;
        sensorAmr->dev[i].pipe = gWaterPort;
        sensorAmr->dev[i].parity = PARITY_NO;
        i++;
    }

    if (gTemperatureAddr != 0)
    {
        sensorAmr->dev[i].addr.modbusAddr = gTemperatureAddr;
        sensorAmr->dev[i].type = SENSOR_H_AND_T;
        sensorAmr->dev[i].pipe = gTemperaturePort;
        sensorAmr->dev[i].parity = PARITY_NO;
        i++;
    }

    if (gSmokeAddr != 0)
    {
        sensorAmr->dev[i].addr.modbusAddr = gSmokeAddr;
        sensorAmr->dev[i].type = SENSOR_SMOKE;
        sensorAmr->dev[i].pipe = gSmokePort;
        sensorAmr->dev[i].parity = PARITY_NO;
        i++;
    }

    if (memcmp(gTransTempAddr, "\x00\x00\x00\x00\x00\x00", 6) != 0)
    {
        memcpy_r(sensorAmr->dev[i].addr.mtrAddr, gTransTempAddr, sizeof(gTransTempAddr));
        sensorAmr->dev[i].type = SENSOR_645_T;
        sensorAmr->dev[i].pipe = gTransTempPort;
        sensorAmr->dev[i].parity = PARITY_EVEN;
        i++;
    }
    
    sensorAmr->devNum = i;
    return 0;
}

uint32 CheckSensorDev(SENSOR_AMR_T *sensorAmr)
{
    uint32 i = 0;
    uint32 flag = 0; /* 该485口有需要抄读的传感器标记 */

    if (sensorAmr->devNum == 0)
    {
        return 1;
    }

    if (sensorAmr->devNum > SENSOR_DEV_MAX_NUM)
    {
        return 1;
    }

    for (i = 0; i < sensorAmr->devNum; i++)
    {
        if (sensorAmr->dev[i].pipe == sensorAmr->pipe)
        {
            flag = 1;
            break;
        }
    }

    if (flag == 0)
    {
        return 1;
    }

    return 0;
}

uint32 InitSensorAmr(SENSOR_AMR_T *sensorAmr, PARITY_TYPE_E parity)
{
    uint32 i = 0;

    for (i = 0; i < sensorAmr->devNum; i++)
    {
        sensorAmr->dev[i].needWriteFlag = 0;
        sensorAmr->dev[i].needMtrFlag = 0;
    }

    return 0;
}

uint32 SensorSendProc(SENSOR_AMR_T *sensorAmr, uint32 devIndex)
{
    SENSOR_DEV_INFO_T *dev = &sensorAmr->dev[devIndex];

    return SendSensorPkt(sensorAmr->pipe, dev, sensorAmr->commPara);
}

uint32 SensorRecvProc(SENSOR_AMR_T *sensorAmr, uint32 devIndex)
{
    //uint32 i = 0;
    uint32 mtrAgainCnt = MTR_AGAIN_CNT;
    //uint32 devIndex = SENSOR_DEV_MAX_NUM;
    SENSOR_RECV_PDU_U pdu = {{0}};
    SENSOR_DEV_INFO_T *dev = &sensorAmr->dev[devIndex];
    TASK_FMT_DEBUG(sensorAmr->pipe, COMM_LOG, "SensorRecvProc\n");
    while (1)
    {
        //for (i = 0; i < MAX_ATTEMPT; i++)
        //{
        usleep(10000); /* 睡10ms */

        if (RecvSensorPkt(sensorAmr->pipe, dev, &pdu, sensorAmr->commPara) == 0)
        {
            if (FillDevResult(sensorAmr->pipe, dev, &pdu, sensorAmr->DBClientHandle) == 0)
            {
                break;
            }
        }
        //}

        if (mtrAgainCnt == 0)
        {
            TASK_FMT_DEBUG(sensorAmr->pipe, COMM_LOG, "没有重抄次数\n");
            break;
        }
        /* 传感器出现抄读失败，进行重抄 */
        if (dev->needMtrFlag == 1)
        {
            TASK_FMT_DEBUG(sensorAmr->pipe, COMM_LOG, "传感器出现抄读失败，进行重抄\n");
            SendSensorPkt(sensorAmr->pipe, dev, sensorAmr->commPara);
            mtrAgainCnt--;
        }
        else
        {
            TASK_FMT_DEBUG(sensorAmr->pipe, COMM_LOG, "抄读异常\n");
            break;
        }
    }
    TASK_FMT_DEBUG(sensorAmr->pipe, COMM_LOG, "SensorRecvProc end\n");
    return 0;
}

uint32 SensorMtrProc(SENSOR_AMR_T *sensorAmr)
{
    uint32 i = 0;

    for (i = 0; i < sensorAmr->devNum; i++)
    {
        if (sensorAmr->dev[i].pipe != sensorAmr->pipe)
        {
            continue;
        }

        if (InitSensorAmr(sensorAmr, sensorAmr->dev[i].parity) != 0)
        {
            TASK_FMT_DEBUG(sensorAmr->pipe, COMM_LOG, "InitSensorAmr err\n");
            continue;
        }
        TASK_FMT_DEBUG(sensorAmr->pipe, COMM_LOG, "InitSensorAmr\n");

        SensorSendProc(sensorAmr, i);
        SensorRecvProc(sensorAmr, i);
        //CalcCurIndex();
    }

    return 0;
}

uint32 SensorAmr(AmrThreadArg_T *arg, RS485_RS232_PARA *commPara)
{
    //TASK_FMT_DEBUG(arg->CommPipe, COMM_LOG, "SensorAmr\n");
    if (SensorMtrTimeCheck(arg->CommPipe) != 0)
    {
        return 1;
    }

    SENSOR_AMR_T sensorAmr;
    SENSOR_AMR_T *amr = &sensorAmr;
    amr->pipe = arg->CommPipe;
    amr->commPara = commPara;
    amr->DBClientHandle = arg->DBClientHandle;
    //TASK_FMT_DEBUG(amr->pipe, COMM_LOG, "SensorMtrTimeCheck\n");

    ReadSensorConf(amr);
    //TASK_FMT_DEBUG(amr->pipe, COMM_LOG, "ReadSensorConf\n");

    if (CheckSensorDev(amr) != 0)
    {
        return 1;
    }
    TASK_FMT_DEBUG(amr->pipe, COMM_LOG, "CheckSensorDev\n");
    /*if (InitSensorAmr(amr) != 0)
    {
        return 1;
    }
    TASK_FMT_DEBUG(amr->pipe, COMM_LOG, "InitSensorAmr\n");*/
    SensorMtrProc(amr);
    TASK_FMT_DEBUG(amr->pipe, COMM_LOG, "SensorMtrProc\n");
    return 0;
}

/*******************************************************************************
* 函数名称: sensor_addr_set
* 函数功能: 设置传感器地址
* 输入参数:
        str 从配置文件中读出的地址
* 输出参数: addr 传感器地址

* 返 回 值: 
*******************************************************************************/
void sensor_addr_set(uint8 *addr,char* str)
{
    char tem[10] = {0};
    
    if(strlen(str)>2)
    {
        memcpy(tem,str,2);
    }else
    {
        strcpy(tem,"00");
        strcpy(tem+2-strlen(str),str);
    }

    commfun_ASC2BCD((char*)addr,(const char*)tem,2,0);

}

/*******************************************************************************
* 函数名称: sensor_addr_set_6
* 函数功能: 设置传感器地址
* 输入参数: 
        str 从配置文件中读出的地址
* 输出参数: addr 传感器地址 6字节

* 返 回 值: 
*******************************************************************************/
void sensor_addr_set_6(uint8 *addr,char* str)
{
    char tem[20] = {0};
    
    if(strlen(str)>12)
    {
        memcpy(tem,str,12);
    }else
    {
        strcpy(tem,"000000000000");
        strcpy(tem+12-strlen(str),str);
    }

    commfun_ASC2BCD((char*)addr,(const char*)tem,12,0);

}
/*******************************************************************************
* 函数名称: sensor_port_get
* 函数功能: 获取传感器端口
* 输入参数:  str 从配置文件中读出的端口
* 输出参数: 

* 返 回 值: 端口号
*******************************************************************************/
uint8 sensor_port_get(char* str)
{
    uint8 port = 0;
    if(strcmp(str,"RS485-1")==0)
    {
        port = RS485_PORT1;
    }else if(strcmp(str,"RS485-2")==0)
    {
        port = RS485_PORT2;
    }else if(strcmp(str,"RS485-3")==0)
    {
        port = RS485_PORT3;
    }else if(strcmp(str,"RS485-4")==0)
    {
        port = RS485_PORT4;
    }else if(strcmp(str,"RS485-5")==0)
    {
        port = RS485_PORT5;
    }else if(strcmp(str,"RS485-6")==0)
    {
        port = RS485_PORT6;
    }else if(strcmp(str,"RS485-7")==0)
    {
        port = RS485_PORT7;
    }else if(strcmp(str,"RS485-8")==0)
    {
        port = RS485_PORT8;
    }
    return port;
}
/*******************************************************************************
* 函数名称: taskmng_init_file_load_area
* 函数功能: 加载配置文件
* 输入参数: 
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void taskmng_init_file_load_area()
{
    char filename[256] = {0};
    dictionary *ini = NULL;
    char* tmp;
    char curpath[256]={0};
   
    int i;
     //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf(" get err path!\n");
        
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    sprintf(filename, "%s%s", curpath, "taskManager.ini");
    ini = iniparser_load(filename);
    printf("读取配置文件%s \n",filename);
    if(ini!=NULL)
    {
        tmp = iniparser_getstring(ini,"mqtt:MQTTIP","172.17.0.1:1883");
        printf("读取mqttip%s \n",tmp);
        strcpy(mqttIP,tmp);
        mqttport = iniparser_getint(ini,"mqtt:MQTTPORT",1883);
        printf("读取mqttport %d \n",mqttport);
        tmp = iniparser_getstring(ini,"Log:COMM_LOG","/data/app/taskManager/log/ctscomm.log");
        printf("CommLog %s \n",tmp);
        strcpy(CommLog,tmp);
        tmp = iniparser_getstring(ini,"Log:TASK_LOG","/data/app/taskManager/log/ctstask.log");
        printf("TaskLog %s \n",tmp);
        strcpy(TaskLog,tmp);
        tmp = iniparser_getstring(ini,"Log:REC_LOG","/data/app/taskManager/log/ctsrec.log");
        printf("RecLog %s \n",tmp);
        strcpy(RecLog,tmp);
        tmp = iniparser_getstring(ini,"Log:UP_LOG","/data/app/taskManager/log/ctsup.log");
        printf("UpLog %s \n",tmp);
        strcpy(UpLog,tmp);
        tmp = iniparser_getstring(ini,"Log:RELAY_LOG","/data/app/taskManager/log/ctsrelay.log");
        printf("RelayLog %s \n",tmp);
        strcpy(RelayLog,tmp);
        tmp = iniparser_getstring(ini,"Log:MQTT_LOG","/data/app/taskManager/log/ctsmqtt.log");
        printf("MqttLog %s \n",tmp);
        strcpy(MqttLog,tmp);
        tmp = iniparser_getstring(ini,"Log:TG_LOG","/data/app/taskManager/log/ctstg.log");
        printf("TgLog %s \n",tmp);
        strcpy(TgLog,tmp);
        tmp = iniparser_getstring(ini,"Dir:SOCKET_DIR","/usr/local/extapps/taskManager/socket");
        printf("SocketDir %s \n",tmp);
        strcpy(SocketDir,tmp);
        tmp = iniparser_getstring(ini,"Dir:PRIVATE_DIR","/usr/local/extapps/taskManager/private");
        printf("PrivateDir %s \n",tmp);
        strcpy(PrivateDir,tmp);
        
        tmp = iniparser_getstring(ini,"HUNAN:S_Temp_Addr","00");
        sensor_addr_set(&gTemperatureAddr,tmp);
        tmp = iniparser_getstring(ini,"HUNAN:S_Temp_Port","RS485-2");
        gTemperaturePort = sensor_port_get(tmp);
        TASK_FMT_DEBUG(-1,UP_LOG,"温度传感器地址 %02x port %d  \n",gTemperatureAddr,gTemperaturePort);
    
        tmp = iniparser_getstring(ini,"HUNAN:S_Smoke_Addr","00");
        sensor_addr_set(&gSmokeAddr,tmp);
        tmp = iniparser_getstring(ini,"HUNAN:S_Smoke_Port","RS485-2");
        gSmokePort = sensor_port_get(tmp);
        TASK_FMT_DEBUG(-1,UP_LOG,"烟雾传感器地址 %02x port %d  \n",gSmokeAddr,gSmokePort);

        tmp = iniparser_getstring(ini,"HUNAN:S_Water_Addr","00");
        sensor_addr_set(&gWaterAddr,tmp);
        tmp = iniparser_getstring(ini,"HUNAN:S_Water_Port","RS485-2");
        gWaterPort = sensor_port_get(tmp);
        TASK_FMT_DEBUG(-1,UP_LOG,"水浸传感器地址 %02x port %d  \n",gWaterAddr,gWaterPort);
        
        tmp = iniparser_getstring(ini,"HUNAN:S_TransTemp_Addr","000000000000");
        sensor_addr_set_6(gTransTempAddr,tmp);
        tmp = iniparser_getstring(ini,"HUNAN:S_TransTemp_Port","RS485-2");
        gTransTempPort = sensor_port_get(tmp);
        TASK_FMT_DEBUG(-1,UP_LOG,"变压器桩头温度传感器地址 %02x%02x%02x%02x%02x%02x port %d  \n",gTransTempAddr[0],
            gTransTempAddr[1],gTransTempAddr[2],gTransTempAddr[3],gTransTempAddr[4],gTransTempAddr[5],gTransTempPort);
        
        iniparser_freedict(ini);
    }
    diag_init(filename, TRUE);
    //装载安全模式
    memset(filename,0x00,sizeof(filename));
    sprintf(filename, "%s", "/data/app/desktopGui/safemode.ini");
    ini = iniparser_load(filename);
    if(ini!=NULL)
    {
        gSafeButton= iniparser_getint(ini,"SAFE_MODE:MAC_MODE",1);
        TASK_FMT_DEBUG(-1,UP_LOG,"gSafeButton %d  \n",gSafeButton);
        gSafeButtonHPLC = iniparser_getint(ini,"SAFE_MODE:HPLC_MAC_MODE",0);
        TASK_FMT_DEBUG(-1,UP_LOG,"gSafeButtonHPLC %d  \n",gSafeButtonHPLC);
        gRouterStudy = iniparser_getint(ini,"SAFE_MODE:ROUTER_STATE_STUDY",1);
        TASK_FMT_DEBUG(-1,UP_LOG,"gRouterStudy %d  \n",gRouterStudy);
        iniparser_freedict(ini);
    }
    else
    {
        gSafeButton=1;
        TASK_FMT_DEBUG(-1,UP_LOG,"没有读到gSafeButton %d  \n",gSafeButton);
        gSafeButtonHPLC = 0;
        TASK_FMT_DEBUG(-1,UP_LOG,"gSafeButtonHPLC %d  \n",gSafeButtonHPLC);
        gRouterStudy = 1;
        TASK_FMT_DEBUG(-1,UP_LOG,"gRouterStudy %d  \n",gRouterStudy);
    }
    printf("读取配置文件%s \n",filename);
    printf("加载配置文件结束！\n");
}

void event_buf_pack(uint8 pipe,uint16 pn,EVENTDATA_T *eventdata)
{
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00,};
    uint16 i;
    uint16 rptLen = 25;
    uint8 Addr[6]={0};
    time_t timenow = time(NULL);
    rptBuff[rptLen++] = 1;  
    memcpy_r(&rptBuff[rptLen],&eventdata->mainoad.value,4);
    rptLen+=4;
    rptBuff[rptLen++] = eventdata->oadnum;
    for(i=0;i<eventdata->oadnum;i++)
    {
        memcpy_r(&rptBuff[rptLen],&eventdata->oads[i].value,4);
        rptLen+=4;
    }
    rptBuff[rptLen++] = 1;  
    rptBuff[rptLen++] = 1;  
    rptBuff[rptLen++] = DT_TSA;
    rptBuff[rptLen++] = 7;
    rptBuff[rptLen++] = 5;
    taskmng_mtr_addr_get(pn,LOGIC_ID_YC,Addr);
    memcpy_r(&rptBuff[rptLen],Addr,6);
    rptLen+=6;
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    rptBuff[rptLen++] =DT_ARRAY;
    rptBuff[rptLen++] = eventdata->oadnum;
    for(i=0;i<eventdata->oadnum;i++)
    {
        rptBuff[rptLen++] = eventdata->oadtype[i];
        if(eventdata->oadlen[i] == 0)
            continue;
        memcpy(&rptBuff[rptLen],eventdata->oadbuf[i],eventdata->oadlen[i]);
        rptLen+=eventdata->oadlen[i];
    }
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,PowerOnOffOad);
}

/*******************************************************************************
* 函数名称: event_temperature_seq
* 函数功能: 解析断路器环境温度越限事件新增次数
* 输入参数: pipe 端口号
        buf 645事件报文
        len 报文长度

* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void event_temperature_seq(uint8 pipe,uint8 *buf,uint16 len)
{
    if(len==0)
    {
        gEventTemperatureSeq[pipe] = 1;
        return;
    }
    gEventTemperatureSeq[pipe] = commfun_BcdToHex(buf[0]);
}

/*******************************************************************************
* 函数名称: event_protect_seq
* 函数功能: 解析保护动作事件新增次数
* 输入参数: pipe 端口号
        buf 645事件报文
        len 报文长度

* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void event_protect_seq(uint8 pipe,uint8 *buf,uint16 len)
{
    if(len==0)
    {
        gEventProtectSeq[pipe] = 1;
        return;
    }
    gEventProtectSeq[pipe] = commfun_BcdToHex(buf[0]);
}

/*******************************************************************************
* 函数名称: event_switchchange_seq
* 函数功能: 解析闸位变化事件新增次数
* 输入参数: pipe 端口号
        buf 645事件报文
        len 报文长度

* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void event_switchchange_seq(uint8 pipe,uint8 *buf,uint16 len)
{
    if(len==0)
    {
        gEventSwitchChangeSeq[pipe] = 1;
        return;
    }
    gEventSwitchChangeSeq[pipe] = commfun_BcdToHex(buf[0]);
}

/*******************************************************************************
* 函数名称: temperature_event_save
* 函数功能: 解析断路器温度告警事件单元
* 输入参数: pipe 端口号
        buf 645事件报文
        len 报文长度
        pn 断路器序号
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void temperature_event_save(uint8 pipe,uint8 *buf,uint16 len,uint16 pn)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"temperature_event_save \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,buf, len);
    if(len<11)
    {
        return;
    }
    uint16 index = 0;
    uint8 flag;     //告警标志
    uint8 reason;  //告警原因
    uint16 year;
    DateTimeHex_t eventtime;
    uint16 temperatue;
    EVENTDATA_T eventdata;
    memset(&eventtime,0x00,sizeof(DateTimeHex_t));
    memset(&eventdata,0x00,sizeof(EVENTDATA_T));
    int msindex = -1;
    msindex = taskmng_ms_index_get(pipe,pn,LOGIC_ID_YC);
    if(msindex == -1)
    {
        return;
    }
    //bit0有效 1发生 0恢复
    flag = buf[index++]&0x01;
    reason = buf[index++];      //故障原因
    reason &= 0x3f;
    eventtime.sec = commfun_BcdToHex(buf[index++]);
    eventtime.min = commfun_BcdToHex(buf[index++]);
    eventtime.hour = commfun_BcdToHex(buf[index++]);
    eventtime.day = commfun_BcdToHex(buf[index++]);
    eventtime.month = commfun_BcdToHex(buf[index++]);
    year = commfun_BcdToHex(buf[index++])+2000;
    eventtime.year_h = year>>8;
    eventtime.year_l = year&0x00ff;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8 *)&temperatue,2,FALSE);
    index+=2;
    //只判断表箱温度越限
    if(reason!=0x0020 &&reason!=0x0021)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"reason %04x 不处理\n",reason);
        return;
    }
    eventdata.mainoad.value = 0x33610200;
    eventdata.oadnum = 4;

    eventdata.oads[0].value = 0x201e0200;   //发生时间
    eventdata.oads[1].value = 0x20200200;   //结束时间
    eventdata.oads[2].value = 0x20220200;  //事件序号
    eventdata.oads[3].value = 0x26000200;  //温度
    
    eventdata.oadtype[0] = DT_DATETIME_S;
    eventdata.oadlen[0] = sizeof(DateTimeHex_t);
    if(flag==1)
    {
//        //发生
//        if(pAmrThreadArg->pMtrAmrInfo[msindex].temperaturestate==1)
//        {
//            TASK_FMT_DEBUG(pipe, RELAY_LOG, "重复上报温度告警事件 \n");
//            return;
//        }
        memcpy(eventdata.oadbuf[0],&eventtime,sizeof(DateTimeHex_t));
        eventdata.oadtype[1] = DT_NULL;
        eventdata.oadlen[1] = 0;
        pAmrThreadArg->pMtrAmrInfo[msindex].temperaturestate = 1;
        pAmrThreadArg->pMtrAmrInfo[msindex].temperaturetime = eventtime;
        pAmrThreadArg->pMtrAmrInfo[msindex].temperatureseq++;
        
    }else
    {   
//        //恢复
//        if(pAmrThreadArg->pMtrAmrInfo[msindex].temperaturestate==0)
//        {
//            TASK_FMT_DEBUG(pipe, RELAY_LOG, "未上报温度告警发生事件 \n");
//            return;
//        }
        memcpy(eventdata.oadbuf[0],&pAmrThreadArg->pMtrAmrInfo[msindex].temperaturetime,sizeof(DateTimeHex_t));
        eventdata.oadtype[1] = DT_DATETIME_S;
        eventdata.oadlen[1] = sizeof(DateTimeHex_t);
        memcpy(eventdata.oadbuf[1],&eventtime,sizeof(DateTimeHex_t));
        pAmrThreadArg->pMtrAmrInfo[msindex].temperaturestate = 0;
    }
    eventdata.oadtype[2] = DT_DOUBLE_LONG_UNSIGNED;
    eventdata.oadlen[2] = 4;
    eventdata.oadbuf[2][3]=gEventTemperatureSeq[pipe];
    //memcpy_r(eventdata.oadbuf[2],&pAmrThreadArg->pMtrAmrInfo[msindex].temperatureseq,4);
    eventdata.oadtype[3] = DT_ARRAY;
    index = 0;
    eventdata.oadbuf[3][index++] = 1;
    eventdata.oadbuf[3][index++] = DT_LONG;
    memcpy_r(&eventdata.oadbuf[3][index],&temperatue,2);
    index+=2;
    eventdata.oadlen[3] = index;
    event_buf_pack( pipe, pn,&eventdata);
}

/*******************************************************************************
* 函数名称: event_protect_save
* 函数功能: 解析断路器保护动作事件单元
* 输入参数: pipe 端口号
        buf 645事件报文
        len 报文长度
        pn 断路器序号
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void event_protect_save(uint8 pipe,uint8 *buf,uint16 len,uint16 pn)
{

    TASK_FMT_DEBUG( pipe, RELAY_LOG,"event_protect_save \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,buf, len);
    if(len<26)
    {
        return;
    }
    uint8 i;
    uint16 index = 0;
    uint8 reason;  //故障原因
    uint8 phase;    //故障相别
    uint16 year;
    int aftercurrent;   //剩余电流
    OOP_WORD3V_T voldata;
    memset(&voldata,0x00,sizeof(OOP_WORD3V_T));
    OOP_INT3V_T curdata;
    memset(&curdata,0x00,sizeof(OOP_INT3V_T));
    DateTimeHex_t eventtime;

    EVENTDATA_T eventdata;
    memset(&eventtime,0x00,sizeof(DateTimeHex_t));
    memset(&eventdata,0x00,sizeof(EVENTDATA_T));
    int msindex = -1;
    msindex = taskmng_ms_index_get(pipe,pn,LOGIC_ID_YC);
    if(msindex == -1)
    {
        return;
    }
    reason = buf[index++];      //故障原因
    reason &= 0x1f;
    phase = buf[index++];       //相别
    eventtime.sec = commfun_BcdToHex(buf[index++]);
    eventtime.min = commfun_BcdToHex(buf[index++]);
    eventtime.hour = commfun_BcdToHex(buf[index++]);
    eventtime.day = commfun_BcdToHex(buf[index++]);
    eventtime.month = commfun_BcdToHex(buf[index++]);
    year = commfun_BcdToHex(buf[index++])+2000;
    eventtime.year_h = year>>8;
    eventtime.year_l = year&0x00ff;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&aftercurrent,4,FALSE);
    index+=2;
    voldata.nNum = 3;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&voldata.nValue[0],2,FALSE);
    index+=2;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&voldata.nValue[1],2,FALSE);
    index+=2;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&voldata.nValue[2],2,FALSE);
    index+=2;
    curdata.nNum = 3;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&curdata.nValue[0],4,FALSE);
    index+=2;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&curdata.nValue[1],4,FALSE);
    index+=2;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&curdata.nValue[2],4,FALSE);
    index+=2;

    eventdata.mainoad.value = 0x35030200;
    eventdata.oadnum = 8;

    eventdata.oads[0].value = 0x201e0200;   //发生时间
    eventdata.oads[1].value = 0x20200200;   //结束时间
    eventdata.oads[2].value = 0x20220200;  //事件序号
    eventdata.oads[3].value = 0x26060200;  //故障原因
    eventdata.oads[4].value = 0x26090200;   //故障相别
    eventdata.oads[5].value = 0x20012500;   //跳闸前剩余电流值
    eventdata.oads[6].value = 0x20002200;   // 跳闸前电压
    eventdata.oads[7].value = 0x20012200;   //跳闸前电流

    eventdata.oadtype[0] = DT_DATETIME_S;
    eventdata.oadlen[0] = sizeof(DateTimeHex_t);
    memcpy(eventdata.oadbuf[0],&eventtime,sizeof(DateTimeHex_t));

    eventdata.oadtype[1] = DT_DATETIME_S;
    eventdata.oadlen[1] = sizeof(DateTimeHex_t);
    memcpy(eventdata.oadbuf[1],&eventtime,sizeof(DateTimeHex_t));

    eventdata.oadtype[2] = DT_DOUBLE_LONG_UNSIGNED;
    eventdata.oadlen[2] = 4;
    eventdata.oadbuf[2][3]=gEventProtectSeq[pipe];
    
    eventdata.oadtype[3] = DT_UNSIGNED;
    eventdata.oadlen[3] = 1;
    memcpy(eventdata.oadbuf[3],&reason,1);
    //相别bit0置1表示A相 bit1置1表示B相 bit2置1表示C相
    eventdata.oadtype[4] = DT_BIT_STRING;
    eventdata.oadlen[4] = 2;
    eventdata.oadbuf[4][0] = 8;
    eventdata.oadbuf[4][1] = phase;

    eventdata.oadtype[5] = DT_DOUBLE_LONG;
    eventdata.oadlen[5] = 4;
    memcpy_r(eventdata.oadbuf[5],&aftercurrent,4);

    eventdata.oadtype[6] = DT_ARRAY;
    index = 0;
    eventdata.oadbuf[6][index++] = 3;
    for(i=0;i<3;i++)
    {
        eventdata.oadbuf[6][index++] =DT_LONG_UNSIGNED;
        memcpy_r(&eventdata.oadbuf[6][index],&voldata.nValue[i],2);
        index+=2;
    }
    eventdata.oadlen[6] = index;

    eventdata.oadtype[7] = DT_ARRAY;
    index = 0;
    eventdata.oadbuf[7][index++] = 3;
    for(i=0;i<3;i++)
    {
        eventdata.oadbuf[7][index++] =DT_DOUBLE_LONG;
        memcpy_r(&eventdata.oadbuf[7][index],&curdata.nValue[i],4);
        index+=4;
    }
    eventdata.oadlen[7] = index;

    event_buf_pack( pipe, pn,&eventdata);
}

/*******************************************************************************
* 函数名称: event_switchchange_save
* 函数功能: 解析断路器闸位变化事件单元
* 输入参数: pipe 端口号
        buf 645事件报文
        len 报文长度
        pn 断路器序号
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void event_switchchange_save(uint8 pipe,uint8 *buf,uint16 len,uint16 pn)
{
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"event_switchchange_save \n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,buf, len);
    if(len<27)
    {
        return;
    }
    uint8 i;
    uint16 index = 0;
    uint8 state;    //闸位变化状态
    uint8 reason;  //故障原因
    uint8 phase;    //故障相别
    uint16 year;
    int aftercurrent;   //剩余电流
    OOP_WORD3V_T voldata;
    memset(&voldata,0x00,sizeof(OOP_WORD3V_T));
    OOP_INT3V_T curdata;
    memset(&curdata,0x00,sizeof(OOP_INT3V_T));
    DateTimeHex_t eventtime;

    EVENTDATA_T eventdata;
    memset(&eventtime,0x00,sizeof(DateTimeHex_t));
    memset(&eventdata,0x00,sizeof(EVENTDATA_T));
    int msindex = -1;
    msindex = taskmng_ms_index_get(pipe,pn,LOGIC_ID_YC);
    if(msindex == -1)
    {
        return;
    }
    state = buf[index++]; //Bit0-Bit1 有效， 其它位无效。 10： 合分；01： 分合
    state &= 0x03;
    reason = buf[index++];      //变位原因
    reason &= 0x1f;
    phase = buf[index++];       //相别
    eventtime.sec = commfun_BcdToHex(buf[index++]);
    eventtime.min = commfun_BcdToHex(buf[index++]);
    eventtime.hour = commfun_BcdToHex(buf[index++]);
    eventtime.day = commfun_BcdToHex(buf[index++]);
    eventtime.month = commfun_BcdToHex(buf[index++]);
    year = commfun_BcdToHex(buf[index++])+2000;
    eventtime.year_h = year>>8;
    eventtime.year_l = year&0x00ff;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&aftercurrent,4,FALSE);
    index+=2;
    voldata.nNum = 3;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&voldata.nValue[0],2,FALSE);
    index+=2;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&voldata.nValue[1],2,FALSE);
    index+=2;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&voldata.nValue[2],2,FALSE);
    index+=2;
    curdata.nNum = 3;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&curdata.nValue[0],4,FALSE);
    index+=2;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&curdata.nValue[1],4,FALSE);
    index+=2;
    Dlt645_ComBcdToBin(&buf[index],2,(uint8*)&curdata.nValue[2],4,FALSE);
    index+=2;

    eventdata.mainoad.value = 0x35040200;
    eventdata.oadnum = 9;

    eventdata.oads[0].value = 0x201e0200;   //发生时间
    eventdata.oads[1].value = 0x20200200;   //结束时间
    eventdata.oads[2].value = 0x20220200;  //事件序号
    eventdata.oads[3].value = 0x28000200;   //闸位变化状态
    eventdata.oads[4].value = 0x26080200;  //故障原因
    eventdata.oads[5].value = 0x26090200;   //故障相别
    eventdata.oads[6].value = 0x20012500;   //跳闸前剩余电流值
    eventdata.oads[7].value = 0x20002200;   // 跳闸前电压
    eventdata.oads[8].value = 0x20012200;   //跳闸前电流

    eventdata.oadtype[0] = DT_DATETIME_S;
    eventdata.oadlen[0] = sizeof(DateTimeHex_t);
    memcpy(eventdata.oadbuf[0],&eventtime,sizeof(DateTimeHex_t));

    eventdata.oadtype[1] = DT_DATETIME_S;
    eventdata.oadlen[1] = sizeof(DateTimeHex_t);
    memcpy(eventdata.oadbuf[1],&eventtime,sizeof(DateTimeHex_t));

    eventdata.oadtype[2] = DT_DOUBLE_LONG_UNSIGNED;
    eventdata.oadlen[2] = 4;
    eventdata.oadbuf[2][3]=gEventSwitchChangeSeq[pipe];

    eventdata.oadtype[3] = DT_ENUM;
    eventdata.oadlen[3] = 1;
    if(state == 2)
    {
        eventdata.oadbuf[3][0] = 1;
    }
    if(state == 1)
    {
        eventdata.oadbuf[3][0] = 2;
    }

    
    eventdata.oadtype[4] = DT_UNSIGNED;
    eventdata.oadlen[4] = 1;
    memcpy(eventdata.oadbuf[4],&reason,1);
    //相别bit0置1表示A相 bit1置1表示B相 bit2置1表示C相
    eventdata.oadtype[5] = DT_BIT_STRING;
    eventdata.oadlen[5] = 2;
    eventdata.oadbuf[5][0] = 8;
    eventdata.oadbuf[5][1] = phase;


    eventdata.oadtype[6] = DT_DOUBLE_LONG;
    eventdata.oadlen[6] = 4;
    memcpy_r(eventdata.oadbuf[6],&aftercurrent,4);

    eventdata.oadtype[7] = DT_ARRAY;
    index = 0;
    eventdata.oadbuf[7][index++] = 3;
    for(i=0;i<3;i++)
    {
        eventdata.oadbuf[7][index++] =DT_LONG_UNSIGNED;
        memcpy_r(&eventdata.oadbuf[7][index],&voldata.nValue[i],2);
        index+=2;
    }
    eventdata.oadlen[7] = index;

    eventdata.oadtype[8] = DT_ARRAY;
    index = 0;
    eventdata.oadbuf[8][index++] = 3;
    for(i=0;i<3;i++)
    {
        eventdata.oadbuf[8][index++] =DT_DOUBLE_LONG;
        memcpy_r(&eventdata.oadbuf[8][index],&curdata.nValue[i],4);
        index+=4;
    }
    eventdata.oadlen[8] = index;

    event_buf_pack( pipe, pn,&eventdata);
}

/*******************************************************************************
* 函数名称: taskmng_sensor_param_get
* 函数功能: 根据oad获取传感器参数配置
* 输入参数:  oad 传感器参数对应的oad
        len 地址长度
* 输出参数: port 传感器端口号
        addr 传感器地址
* 返 回 值:  -1 未获取到参数或者参数非法 0 正确
*******************************************************************************/
int taskmng_sensor_param_get(OOP_OAD_U oad,uint8 *port,uint8 *addr,uint8 addrlen)
{
    int   ret;
    uint32 Len;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad = oad;
    NormalOad.infoNum = 0;
    uint8 testaddr[6] = {0x00};
    uint8 tmpport;
    OOP_SENSOR_PARAM_T param;
    memset(&param,0x00,sizeof(OOP_SENSOR_PARAM_T));
    if(addrlen>6)
        return -1;
    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_SENSOR_PARAM_T),(uint8*)&param,&Len);
    if(ret!=0 || Len != sizeof(OOP_SENSOR_PARAM_T))
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "读取传感器参数失败 %08x ret :%d  \n",oad.value, ret);
    }else
    {
        tmpport = taskmng_mtr_pipe_get(&param.port);
        //判断端口是否有效
        if(tmpport>1 && tmpport<10)
        {
            if(param.addr.nNum>=addrlen && memcmp(testaddr,param.addr.value,addrlen)!=0)
            {
                *port = tmpport;
                memcpy(addr,param.addr.value,addrlen);
                TASK_FMT_DEBUG(-1,UP_LOG, "从数据中心获取传感器参数 %08x port %d  \n",oad.value, tmpport);
                TASK_BUF_DEBUG(-1,UP_LOG,addr,addrlen);
                return 0;
            }
        }
    }
    return -1;
}

void taskmng_sensor_param_load()
{
    taskmng_sensor_param_get(gTemperatureOad,&gTemperaturePort,&gTemperatureAddr,1);
    taskmng_sensor_param_get(gSmokeOad,&gSmokePort,&gSmokeAddr,1);
    taskmng_sensor_param_get(gWaterOad,&gWaterPort,&gWaterAddr,1);
    taskmng_sensor_param_get(gTransTempOad,&gTransTempPort,gTransTempAddr,6);
}

void taskmng_HPLC_module_curve_flag_set(uint8 flag)
{
    gHPLCModuleCurveFlag = flag;
}

uint8 taskmng_HPLC_module_curve_flag_get()
{
    //暂时关闭归档 待测试后再启用
    return 0;
    return gHPLCModuleCurveFlag ;
}
/*******************************************************************************
* 函数名称: taskmng_HPLC_module_curve_flag_load
* 函数功能: 获取F2096300hplc模块存储曲线标记
* 输入参数
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void taskmng_HPLC_module_curve_flag_load()
{
    int   ret;
    uint32 Len;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value =0xF2096300;
    NormalOad.infoNum = 0;
    uint8 flag = 0;

    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(flag),&flag,&Len);
    if(ret!=0 || Len != sizeof(flag))
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "加载HPLC模块存储曲线标记失败 ret :%d  \n",ret);
    }else{
        TASK_FMT_DEBUG(-1,UP_LOG, "加载HPLC模块存储曲线标记成功 %d ret :%d  \n",flag,ret);
    }
    //读不到就默认为0
    taskmng_HPLC_module_curve_flag_set(flag&0x01);
    
}

void taskmng_module_curve_store_data_set(OOP_MODULE_CURVE_STORE_DATA_T *data)
{
    if(data==NULL)
        return;
    memcpy(&gModuleCurveStoreData,data,sizeof(OOP_MODULE_CURVE_STORE_DATA_T));
}

void taskmng_module_curve_store_data_get(OOP_MODULE_CURVE_STORE_DATA_T *data)
{
    memcpy(data,&gModuleCurveStoreData,sizeof(OOP_MODULE_CURVE_STORE_DATA_T));
}

uint8 taskmng_all_data_di_get(const OOP_TO_DATA_ID_PARAM_T *data,uint8 len,OOP_DATA_ID_PARAM_T *des)
{
    uint8 i = 0;
    uint8 result;
    for(i=0;i<len;i++)
    {
        result+=data[i].datalen;
        des[i].dataid = data[i].dataid;
        des[i].datalen = data[i].datalen;
    }

    return result;
}


/*******************************************************************************
* 函数名称: taskmng_module_curve_store_data_load
* 函数功能: 获取F2096500HPLC-CCO 模块曲线存储数据项
* 输入参数
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void taskmng_module_curve_store_data_load()
{
    int   ret;
    uint32 Len;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value =0xF2096500;
    NormalOad.infoNum = 0;
    OOP_MODULE_CURVE_STORE_DATA_T data;
    memset(&data,0x00,sizeof(OOP_MODULE_CURVE_STORE_DATA_T));

    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_MODULE_CURVE_STORE_DATA_T),(uint8*)&data,&Len);
    if(ret!=0 || Len != sizeof(OOP_MODULE_CURVE_STORE_DATA_T))
    {
        TASK_FMT_DEBUG(-1,UP_LOG, "加载HPLC模块存储曲线存储数据项失败 ret :%d 使用默认方案 \n",ret);
        data.flag = TRUE;
        data.period = 15;   //默认15分钟
        data.dataidstoretab645.prtl = PRTL_07;
        data.dataidstoretab645.singledatanum = gDataIdParam_single_645_len;
        data.dataidstoretab645.singledatasize = taskmng_all_data_di_get(gDataIdParam_single_645,gDataIdParam_single_645_len,data.dataidstoretab645.singledataparam);
        data.dataidstoretab645.threedatanum = gDataIdParam_three_645_len;
        data.dataidstoretab645.threedatasize = taskmng_all_data_di_get(gDataIdParam_three_645,gDataIdParam_three_645_len, data.dataidstoretab645.threedataparam);
        data.dataidstoretab698.prtl = PRTL_OOP;
        data.dataidstoretab698.singledatanum = gDataIdParam_single_oop_len;
        data.dataidstoretab698.singledatasize = taskmng_all_data_di_get(gDataIdParam_single_oop,gDataIdParam_single_oop_len,data.dataidstoretab698.singledataparam);
        data.dataidstoretab698.threedatanum = gDataIdParam_three_oop_len;
        data.dataidstoretab698.threedatasize = taskmng_all_data_di_get(gDataIdParam_three_oop,gDataIdParam_three_oop_len,data.dataidstoretab698.threedataparam);

    }else{
        TASK_FMT_DEBUG(-1,UP_LOG, "加载HPLC模块存储曲线存储数据项成功  \n");
    }
    //读不到就默认填空
    taskmng_module_curve_store_data_set(&data);
    
}

/*******************************************************************************
* 函数名称: taskmng_curve_store_data_buf_pack
* 函数功能: 组设置STA模块曲线配置3762报文的数据域
* 输入参数
* 输出参数: databuf 组的数据域
* 返 回 值:  数据长度
*******************************************************************************/
uint16 taskmng_curve_store_data_buf_pack(uint8 *databuf,uint8 prtl)
{
    uint16 index = 0;
    uint8 i;
    OOP_MODULE_CURVE_STORE_DATA_T data;
    memset(&data,0x00,sizeof(OOP_MODULE_CURVE_STORE_DATA_T));
    taskmng_module_curve_store_data_get(&data);
    //当“启用/禁用”标识为“禁用”时，数据内容为：00 00 00 00 00 00 00 00 00
    if(data.flag == FALSE)
    {
        memset(databuf,0x00,9);
        index=9;
        return index;
    }
    databuf[index++] = data.flag;
    databuf[index++] = data.period;
    databuf[index++] =prtl;
    
    if(prtl==PRTL_07)
    {
        //单相表
        databuf[index++] = 0;
        databuf[index++] = data.dataidstoretab645.singledatanum;
        databuf[index++] = data.dataidstoretab645.singledatasize;
        for(i=0;i<data.dataidstoretab645.singledatanum;i++)
        {
            memcpy_r(&databuf[index],&data.dataidstoretab645.singledataparam[i].dataid,4);
            index+=4;
            databuf[index++] = data.dataidstoretab645.singledataparam[i].datalen;
        }
        //三相表
        databuf[index++] = 1;
        databuf[index++] = data.dataidstoretab645.threedatanum;
        databuf[index++] = data.dataidstoretab645.threedatasize;
        for(i=0;i<data.dataidstoretab645.threedatanum;i++)
        {
            memcpy_r(&databuf[index],&data.dataidstoretab645.threedataparam[i].dataid,4);
            index+=4;
            databuf[index++] = data.dataidstoretab645.threedataparam[i].datalen;
        }
        
    }else if(prtl==PRTL_OOP)
    {
        //单相表
        databuf[index++] = 0;
        databuf[index++] = data.dataidstoretab698.singledatanum;
        databuf[index++] = data.dataidstoretab698.singledatasize;
        for(i=0;i<data.dataidstoretab698.singledatanum;i++)
        {
            memcpy_r(&databuf[index],&data.dataidstoretab698.singledataparam[i].dataid,4);
            index+=4;
            databuf[index++] = data.dataidstoretab698.singledataparam[i].datalen;
        }
        //三相表
        databuf[index++] = 1;
        databuf[index++] = data.dataidstoretab698.threedatanum;
        databuf[index++] = data.dataidstoretab698.threedatasize;
        for(i=0;i<data.dataidstoretab698.threedatanum;i++)
        {
            memcpy_r(&databuf[index],&data.dataidstoretab698.threedataparam[i].dataid,4);
            index+=4;
            databuf[index++] = data.dataidstoretab698.threedataparam[i].datalen;
        }
    }else
        return 0;
    return index;
}
/*******************************************************************************
* 函数名称: taskmng_router_state_frame_unpack
* 函数功能: 解析查询路由状态回码
* 输入参数:buf 报文
    len 报文长度
* 输出参数: 
* 返 回 值: 0 收到确认帧 -1收到否认
*******************************************************************************/
int taskmng_curve_store_data_unpack(uint8 *buf,uint16 len)
{
    int ret=0;
    uint16 frameend;
    uint16 framelen;
    uint8 lenoffset;
    AppData_T appdata;
    memset(&appdata,0x00,sizeof(AppData_T));
    framelen = get_len_offset(buf, &lenoffset);
    if(framelen == 0)
    {
        return -1;
    }
    ret = taskmng_frame_3762_format_check(framelen,buf+lenoffset,&frameend);
    if(ret == 0 )
    {
        //识别出3762 开始解析
        ret = taskmng_frame_3762_unpack(framelen,buf+lenoffset,&appdata);
        if(ret == 0&&appdata.AFN ==0x00&&appdata.Fn == 0x01)
        {
            return 0;
        }
    }

    return -1;


}

/*******************************************************************************
* 函数名称: taskmng_curve_store_data_sync
* 函数功能: 透传3762和CCO同步STA模块曲线配置 AFN=05H Fn=103
* 输入参数
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
int taskmng_curve_store_data_sync(uint8 pipe,uint8 prtl)
{
    uint8 payload[500]={0};
    uint16 index = 0;
    uint16 overtime = 6;
    uint8 frame[200]={0};
    uint16 framelen;
    uint8 lenoffset;
    int ret;
    uint16 recvlen;
    uint8 recvbuf[500]={0};
    AppData_T appdata;
    memset(&appdata,0x00,sizeof(AppData_T));
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "开始同步STA模块曲线配置 prtl %d \n",prtl);
    //组3762报文
    appdata.AFN = 5;
    appdata.Fn = 103;
    appdata.DataUnitLen = taskmng_curve_store_data_buf_pack(appdata.DataUnitBuff,prtl);
    framelen = taskmng_3762_frame_pack(appdata,frame);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    payload[index++] = 0;
    index++;
    memcpy_r(&payload[index],(uint8*)&overtime,2);
    index+=2;
    memcpy_r(&payload[index],(uint8*)&overtime,2);
    index+=2;
    lenoffset = set_len_offset(framelen,&payload[index] );
    index+=lenoffset;
    memcpy(&payload[index],frame,framelen);
    index+=framelen;
    ret = taskmng_msg_fun_deal(pipe,0x0003,0x0033,index,payload,&recvlen,recvbuf);
    if(ret>0)
    {
        ret = taskmng_curve_store_data_unpack(recvbuf,recvlen);
        if(ret ==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "同步STA模块曲线配置成功 prtl %d \n",prtl);
            return 0;
        }
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "同步STA模块曲线配置失败 prtl %d \n",prtl);
    return -1;
}
/*******************************************************************************
* 函数名称: taskmng_curve_sync_check
* 函数功能: 检查是否要同步 档案同步后要执行 收到变更事件 模块重新插拔后
* 输入参数  pipe 端口号
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void taskmng_curve_sync_check(uint8 pipe)
{
    uint8 flag = taskmng_HPLC_module_curve_flag_get();
    if(flag == 0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "模块曲线标记未开启\n");
        return;
    }
    taskmng_curve_store_data_sync(pipe,PRTL_07);
    taskmng_curve_store_data_sync(pipe,PRTL_OOP);
}

void taskmng_curve_sync_flag_set(uint8 pipe,uint8 flag)
{
    if(pipe>1)
        return;
    gCurveStoreSyncFlag[pipe] = flag;

}

uint8 taskmng_curve_sync_flag_get(uint8 pipe)
{
    return gCurveStoreSyncFlag[pipe];
}

/*******************************************************************************
* 函数名称: taskmng_sta_curve_state_get_send 
* 函数功能:获取STA模块配置状态组帧发送
* 输入参数:pipe 端口号
        
* 输出参数: 
* 返 回 值: 0 所有发送完毕，任务结束 1 切换接收态
*******************************************************************************/

int taskmng_sta_curve_state_get_send(uint8 pipe)
{
    STA_CURVE_STATE_GET_INFO_T *info = &gAmrThreadArg[pipe].STACurveInfo;
    uint8 payload[2000] = {0};
    uint16 index = 0;
    uint8 frame[200] = {0};
    uint16 framelen;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    uint16 overtime = 30;
    AppData_T appdata;
    memset(&appdata,0x00,sizeof(AppData_T));
    if(info->failcnt>=3)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_sta_curve_state_get_send 失败次数过多 任务结束\n");
        return 0;
    }

    appdata.AFN = 0x10;
    appdata.Fn = 103;
    appdata.DataUnitLen = 3;
    memcpy(appdata.DataUnitBuff,&info->beginseq,2);
    appdata.DataUnitBuff[2] = MAX_STA_CURVE_STATE_GET_NUM;
    framelen = taskmng_3762_frame_pack(appdata,frame);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "查询sta模块配置 起始序号 %d \n",info->beginseq);
    TASK_BUF_DEBUG( pipe, RELAY_LOG,frame,framelen);

    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    payload[index++] = 0;
    memcpy_r(&payload[index],(uint8*)&overtime,2);
    index+=2;
    memcpy_r(&payload[index],(uint8*)&overtime,2);
    index+=2;
    payload[index++] = framelen;
    memcpy(&payload[index],frame,framelen);
    index+=framelen;
    
    taskmng_msg_data_pack(pipe,0x0003,0x0033,index,payload,&info->sendseq);
    return 1;

}
/*******************************************************************************
* 函数名称: taskmng_sta_curve_state_get_recv 
* 函数功能:收到回码后的处理
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数: 
* 返 回 值: 0 数据处理完毕 -1 回码为空 1 收到个数小于30个表示所有节点查询完毕
*******************************************************************************/

int taskmng_sta_curve_state_get_recv(uint8 pipe,uint8 *buf,uint16 buflen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    STA_CURVE_STATE_GET_INFO_T *info = &pAmrThreadArg->STACurveInfo;
    int ret=0;
    uint16 frameend;
    uint16 framelen;
    uint8 lenoffset;
    uint16 totalnum;
    uint8 i,num;
    uint16 index = 0;
    AppData_T appdata;
    memset(&appdata,0x00,sizeof(AppData_T));
    uint8 state;
    uint8 addr[6]={0};
    uint16 pn;
    int16 msindex;
    framelen = get_len_offset(buf, &lenoffset);
    if(framelen == 0)
    {
        return -1;
    }
    ret = taskmng_frame_3762_format_check(framelen,buf+lenoffset,&frameend);
    if(ret == 0 )
    {
        //识别出3762 开始解析
        ret = taskmng_frame_3762_unpack(framelen,buf+lenoffset,&appdata);
        if(ret == 0&&appdata.AFN ==0x10&&appdata.Fn ==103)
        {
            //对3762数据域进行解析
            memcpy(&totalnum,appdata.DataUnitBuff,2);
            index+=2;
            num = appdata.DataUnitBuff[index++];
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "节点总数量%d      本次数量    %d  \n",totalnum,num);
            if(appdata.DataUnitLen<(3+num*(6+1)))
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据长度不对 \n");
                return -1;
            }
            
            for(i=0;i<num;i++)
            {
                //地址
                memcpy(addr,&appdata.DataUnitBuff[index],6);
                index+=6;
                state = appdata.DataUnitBuff[index++];

                pn = taskmng_pn_get_from_addr(addr,LOGIC_ID_YC);
                if(pn==0)
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "无法在档案中找到该地址 \n");
                    TASK_BUF_DEBUG( pipe, RELAY_LOG, addr,6);
                    continue;
                }
                msindex = taskmng_ms_index_get(pipe,pn,LOGIC_ID_YC);
                if(msindex!=-1)
                {
                    pAmrThreadArg->pMtrAmrInfo[msindex].stacurvestate = state;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表 %d 模块配置状态 %d \n",pn,state);
                }
            }
            info->beginseq+=num;
            if(num<MAX_STA_CURVE_STATE_GET_NUM)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "所有节点查询完毕 \n");
                return 1;
            }
            return 0;
        }
    }
    
    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_sta_curve_state_get 
* 函数功能:查询STA模块配置状态，自组3762 10F103 上电 过日 档案变更时执行
* 输入参数:pipe 端口号
        
* 输出参数: 
* 返 回 值: 1 处理完成 -1 未完成
*******************************************************************************/
int taskmng_sta_curve_state_get(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    STA_CURVE_STATE_GET_INFO_T *info = &pAmrThreadArg->STACurveInfo;
    uint16 i;
    int ret;
    static time_t sendtime[PLC_NUM];
    uint16 RecvLen;
    uint8 RecvBuf[2000]={0};
    MESSAGE_INFO_T* MsgInfo = NULL;
    switch(pAmrThreadArg->TotalState.GetSTACurveState)
    {
    case INNER_RTS_IDLE:
    {
        if(taskmng_HPLC_module_curve_flag_get(pipe)==0)
        {
            return 1;
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "开始查询STA模块配置状态\n");
        memset(info,0x00,sizeof(STA_CURVE_STATE_GET_INFO_T));
        for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
        {
            pAmrThreadArg->pMtrAmrInfo[i].stacurvestate = 0;
        }
        pAmrThreadArg->TotalState.GetSTACurveState = INNER_RTS_SEND;
    }
        break;
    case INNER_RTS_SEND:
    {
        ret = taskmng_sta_curve_state_get_send(pipe);
        if(ret == 1)
        {
            sendtime[pipe] = time(NULL);
            pAmrThreadArg->TotalState.GetSTACurveState = INNER_RTS_RECV;
        }else
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_sta_curve_state_get 任务结束  \n");
            pAmrThreadArg->TotalState.GetSTACurveState = INNER_RTS_IDLE;
            return 1;
        }
    }
        break;
    case INNER_RTS_RECV:
    {
        RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuf );
        if(RecvLen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
            if(MsgInfo->index!=info->sendseq)
            {
                break;
            }
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_sta_curve_state_get 收到消息  \n");
            TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);
            ret = taskmng_sta_curve_state_get_recv(pipe,MsgInfo->playload, MsgInfo->msglen);
            if(ret ==0)
            {
                pAmrThreadArg->TotalState.GetSTACurveState = INNER_RTS_SEND;
            }else if(ret == -1)
            {
                pAmrThreadArg->TotalState.GetSTACurveState = INNER_RTS_RETRY;
            }else 
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_sta_curve_state_get 任务结束  \n");
                pAmrThreadArg->TotalState.GetSTACurveState = INNER_RTS_IDLE;
                return 1;
            }
        }else
        {
            if(time(NULL)-sendtime[pipe] >30 )
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "起始序号%d 超时 \n",info->beginseq);
            
                pAmrThreadArg->TotalState.GetSTACurveState = INNER_RTS_RETRY;
            }
        }
    }
        break;
    case INNER_RTS_RETRY:
    {
        info->failcnt++;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "失败次数 %d \n",info->failcnt);
        pAmrThreadArg->TotalState.GetSTACurveState = INNER_RTS_SEND;
    }
        break;
    default:
        break;
    }

    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_meter_frame_make_sta_curve
* 函数功能: 湖南模块曲线抄读组帧 07规约
* 输入参数: Pipe 端口号
         StartTime  开始时间
         FrameSeqInfo 帧信息

* 输出参数: DataBuff 组好的抄表帧
* 返 回 值: 抄表帧长度
*******************************************************************************/
uint16 taskmng_meter_frame_make_sta_curve(uint8 Pipe,uint8 seqindex,time_t StartTime,FRAME_SEQ_INFO_T FrameSeqInfo,uint8 *DataBuff,uint16 taskid,uint8 logicid)
{
    uint16 framelen = 0;
    uint16 index = 0;
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    SCHEME_TASK_RUN_INFO SchemeTaskInfo ;
    taskmng_task_info_get(taskid,logicid,&SchemeTaskInfo);
    MeterPrtlFmt_T  MeterPrtlData;
    memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
    DateTimeBcd_t   proStartTime;
    if(taskmng_HPLC_module_curve_flag_get()==1&&FrameSeqInfo.PrtlMode ==PRTL_07 && SchemeTaskInfo.CsdTask.RoadTask[0].MainOad.nObjID == 0x5002&&
        pAmrThreadArg->pMtrAmrInfo[FrameSeqInfo.MsIndex].stacurvestate ==1)
    {
        memcpy(MeterPrtlData.MtrAddr, FrameSeqInfo.MeterAddr, 6);
        MeterPrtlData.CtrlCode = 0x11;
        memcpy(MeterPrtlData.DataBuff, FrameSeqInfo.DiInfo.DI, 4);
        index+=4;
        DT_Time2DateTimeBCD( StartTime, &proStartTime);
        memcpy_r(&MeterPrtlData.DataBuff[index], &proStartTime.year_l, 5);
        index+=5;
        MeterPrtlData.DataBuff[index++]=taskmng_curve_point_get( taskid, logicid); 
        MeterPrtlData.DataLen = index;
        framelen = taskmng_meter_prtl_645_pack(DataBuff, MeterPrtlData);
    }
    return framelen;
}

uint16 taskmng_sta_curve_get_msg_pack(uint8 pipe,uint8 *payload,uint8 *addr,uint8 *buf,uint16 buflen)
{
    uint16 index = 0;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe); 
    
    memcpy_r(&payload[index],(uint8*)&pipeOAD.value,4);
    index+=4;
    index+=2; //通信控制字两个字节
    memcpy_r(&payload[index],addr,6);
    index+=6;
    if(buflen<0x80)
    {
        payload[index++] = buflen;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(buflen>>8);
        payload[index++] = (uint8)buflen;
    }
    memcpy(&payload[index],buf,buflen);
    index+=buflen;          //主动抄表PayLoad长度
    return index;
}

/*******************************************************************************
* 函数名称: taskmng_amr_report_data_unpack_0071
* 函数功能: 解上报的抄表报文
* 输入参数: PayloadBuf           消息内容
            Len         长度
* 输出参数: AmrReportInfo            抄表数据结构体
* 返 回 值:  
*******************************************************************************/
BOOL taskmng_amr_report_data_unpack_0071(uint8 *PayloadBuf,uint32 Len,AMR_PLC_INFO_REPORT_T  *AmrReportInfo)
{
    uint32 index =0;
    uint8   lenoffset;
    OOP_OAD_U pipeOad ;
    memset(&pipeOad,0x00,sizeof(OOP_OAD_U));
    uint8 addr[6]={0};
    memcpy_r((uint8*)&pipeOad.value, &PayloadBuf[index], 4); //端口号
    index+=4;
    AmrReportInfo->nPortID = taskmng_mtr_pipe_get(&pipeOad);
    if(AmrReportInfo->nPortID>1)
        return FALSE;
    memcpy_r(addr,&PayloadBuf[index], 6);
    index+=6;
    AmrReportInfo->nMeterFilesID = taskmng_pn_get_from_addr(addr,LOGIC_ID_YC);
    if(AmrReportInfo->nMeterFilesID == 0)
    {
        TASK_FMT_TRACE(AmrReportInfo->nPortID, REC_LOG, "上报的抄表地址找不到\n");
        return FALSE;
    }
    AmrReportInfo->nProtocolType = PayloadBuf[index++];
    if(index ==Len) //上报的否认没有数据帧
    {
        AmrReportInfo->nDataBufLen =0;
        return TRUE;
    }
    AmrReportInfo->nDataBufLen= XADR_GetEleNum(&PayloadBuf[index],&lenoffset);
    index+=lenoffset;
    TASK_FMT_TRACE(AmrReportInfo->nPortID, REC_LOG, "taskmng_amr_report_data_unpack pn %d prtl %d  datalen %d \n",AmrReportInfo->nMeterFilesID,AmrReportInfo->nProtocolType,AmrReportInfo->nDataBufLen);
    //TASK_FMT_TRACE(AmrReportInfo->nPortID, REC_LOG, "Len %d index %d lenoffset %d \n",Len,index,lenoffset);
    if(index+AmrReportInfo->nDataBufLen>Len)
        return FALSE;
    memcpy(AmrReportInfo->pDataBuf,&PayloadBuf[index],AmrReportInfo->nDataBufLen);
    index+=AmrReportInfo->nDataBufLen;
    if(index==Len)
        return TRUE;
    if(PayloadBuf[index++]==0x00)
       return TRUE;
    AmrReportInfo->isExitReadingInfo = 1;
    AmrReportInfo->stPlcReadingInfo.nRelayRouterLevel = PayloadBuf[index++];
    AmrReportInfo->stPlcReadingInfo.nChannelIdentifier = PayloadBuf[index++];
    AmrReportInfo->stPlcReadingInfo.nCarrierReadPhase = PayloadBuf[index++];
    AmrReportInfo->stPlcReadingInfo.nCarrierSignalQuality = PayloadBuf[index++];
    AmrReportInfo->stPlcReadingInfo.nCarrierSymbol = PayloadBuf[index++];
    memcpy_r(AmrReportInfo->stPlcReadingInfo.aReserveForOtherUse,&PayloadBuf[index],3);
    index+=3;
    return TRUE;
}


/*******************************************************************************
* 函数名称: taskmng_645_meter_data_save_stacurve
* 函数功能: 存储07表模块曲线
* 输入参数:     pipe        端口号
            pBuf          645回码数据
*           rcvLen        645回码长度
*           MtrIndex      电表索引
*           Tmp_AmrLink   方案任务节点
            RecvRunTask     任务运行时参数
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_645_meter_data_save_stacurve(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8         offset=0;
    DateTimeBcd_t storeTm;
    uint8         PointNum;
    uint16        DataLen;
    time_t        TempTimeSec;
    OOP_TI_T          DelayTi;
    int           i;
    uint8  density;     //密度

    //最少5个字节时间加密度 数据点数
    if(rcvLen<5+2)
    {
        return ;
    }
    memcpy_r(&storeTm.year_l,pBuf,5);
    storeTm.year_h = 0x20;
    storeTm.sec = 0x00;
    if(DT_DateTimeBCD2Time(&storeTm, &TempTimeSec) == FALSE)
        return;
    //判断时间是否正确 如果不正确 则切换抄读方式

    if(TempTimeSec<pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime)
    {
        TASK_FMT_TRACE(Pipe, REC_LOG, " Recvtime %d starttime %d  \n",TempTimeSec,pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime);
#if CURRENT_TO_CURVE_ENABLE
        pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 = 1;
#endif
        return;
    }

    offset += 5;
    density = pBuf[offset++];
    if(density == 1)
    {
        //15分钟
        DelayTi.value = 15;
        DelayTi.unit = 1; 
    }else if(density == 2)
    {
        //15分钟
        DelayTi.value = 30;
        DelayTi.unit = 1; 
    }else if(density == 3)
    {
        //15分钟
        DelayTi.value = 60;
        DelayTi.unit = 1; 
    }else
    {   
        TASK_FMT_TRACE(Pipe, REC_LOG, "数据密度 %d 非法 \n",density);
        return ;
    }
    PointNum = pBuf[offset++];
    DataLen = (rcvLen-7)/PointNum;

    for(i=0;i<PointNum;i++)
    {
         //计算每个时间点的存储时标

        if(i != 0)
            DT_TimeAddInv(&TempTimeSec, DelayTi);

        //无效数据不存储
        if(pBuf[offset] == 0xff && pBuf[offset+1] == 0xff)
        {
            offset += DataLen;
            continue;
        }
        if(Tmp_AmrLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR
            || (Tmp_AmrLink->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN && Tmp_AmrLink->AcqInfo.TI.value == 60))
        {
            if(i%4 != 0)
            {
                offset += DataLen;
                continue;
            }
        }
        DT_Time2DateTimeBCD(TempTimeSec, &storeTm);
        taskmng_645_meter_data_save(Pipe,&pBuf[offset], DataLen, MsIndex, Tmp_AmrLink, RecvRunTask,&storeTm);
        offset += DataLen;
    }
    return ;
}
/**********************************************************************
* @name      : get_power_state
* @brief     ：获取电源状态
* @param[in] ：  
* @param[out]：state: 电源状态，1有电，0 掉电
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int get_power_state(int *state)
{
	POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
	if(NULL == dev)
	{
        TASK_FMT_DEBUG(-1,UP_LOG, "dev is null!\n");
        return -1;
	}
	
	if(dev->power_power_state_get(dev,POWER_MAIN,state) < 0)
	{
        TASK_FMT_DEBUG(-1,UP_LOG, "get power state failed!\n");
		return -1;
	}
    
	hal_device_release((HW_DEVICE *)dev);
	return 1;
}

/*******************************************************************************
* 函数名称: taskmng_event_clock_offset_deal 
* 函数功能:判断是否收到时钟偏差事件上报 68 19 00 00 00 00 00 68 9f 07 34 46 44 33 33 33 33 19 16
* 输入参数:
        buf 消息内容 645报文
        buflen 消息长度

* 输出参数: 
* 返 回 值: 0 识别到时钟偏差事件 -1 未识别出
*******************************************************************************/
int taskmng_event_clock_offset_deal(uint8 pipe,uint8 *buf,uint16 buflen)
{
    static uint32 eventseq = 0 ;
    uint8 addr[6]={0};
    uint16 pn;
    uint8 offsetflag = 0;   //偏差方式 01表示电表超出集中器时间，02表示电能表低于集中器时间
    uint32 offtime; //偏差时间
    uint8 data[100]={0};
    time_t timenow = time(NULL);
    DateTimeHex_t timehex;
    memset(&timehex,0x00,sizeof(DateTimeHex_t));
    EVENTDATA_T eventdata;
    memset(&eventdata,0x00,sizeof(EVENTDATA_T));
    uint8 tmpbuf[100]={0};
    uint32 metertime;   //电表时钟
    if(buflen==19 && buf[8]==0x9F && buf[9]==0x07)
    {
        memcpy(addr,&buf[1],6);
        pn = taskmng_pn_get_from_addr(addr,LOGIC_ID_YC);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "收到时钟偏差事件上报 pn %d\n",pn);
        if(pn == 0)
        {
            return 0;
        }
        memcpy(data,&buf[10],7);
        Pro_ReduceMeterGap(data,7,0x33);
        offsetflag = data[0];
        offtime = commfun_HexToBcd(data[1])+commfun_HexToBcd(data[2])*60+commfun_HexToBcd(data[3])*60*60+
                    commfun_HexToBcd(data[4])*60*60*24+commfun_HexToBcd(data[5])*60*60*24*30+commfun_HexToBcd(data[6])*60*60*24*30*365;

        eventdata.mainoad.value = 0x31050200;
        eventdata.oadnum = 6;

        eventdata.oads[0].value = 0x201e0200;   //发生时间
        eventdata.oads[1].value = 0x20200200;   //结束时间
        eventdata.oads[2].value = 0x20220200;  //事件序号
        eventdata.oads[3].value = 0x20240200;  //事件发生源
        eventdata.oads[4].value = 0x33130206;  //电能表时钟
        eventdata.oads[5].value = 0x33130207;  //终端时钟

        eventdata.oadtype[0] = DT_DATETIME_S;
        eventdata.oadlen[0] = sizeof(DateTimeHex_t);
        DT_Time2DateTimeHex(timenow,&timehex);
        memcpy(eventdata.oadbuf[0],&timehex,sizeof(DateTimeHex_t));

        eventdata.oadtype[1] = DT_NULL;
        eventdata.oadlen[1] = 0;

        eventdata.oadtype[2] = DT_DOUBLE_LONG_UNSIGNED;
        eventdata.oadlen[2] = 4;
        memcpy_r(eventdata.oadbuf[2],&eventseq,sizeof(eventseq));
        eventseq++;

        eventdata.oadtype[3] = DT_TSA;
        eventdata.oadlen[3] = 8;
        tmpbuf[0] = 7;
        tmpbuf[1] = 5;
        memcpy_r(&tmpbuf[2],addr,6);
        memcpy(eventdata.oadbuf[3],tmpbuf,8);

        if(offsetflag ==1)
        {
            metertime = timenow+offtime;
        }else
        {
            metertime = timenow-offtime;
        }
        eventdata.oadtype[4] = DT_DATETIME_S;
        eventdata.oadlen[4] = sizeof(DateTimeHex_t);
        DT_Time2DateTimeHex(metertime,&timehex);
        memcpy(eventdata.oadbuf[4],&timehex,sizeof(DateTimeHex_t));

        eventdata.oadtype[5] = DT_DATETIME_S;
        eventdata.oadlen[5] = sizeof(DateTimeHex_t);
        DT_Time2DateTimeHex(timenow,&timehex);
        memcpy(eventdata.oadbuf[5],&timehex,sizeof(DateTimeHex_t));
        event_buf_pack( pipe, pn,&eventdata);
        return 0;
    }
    return -1;
}

#endif
