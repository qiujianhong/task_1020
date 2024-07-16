
/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_report.h"
#include "ac_lib.h"
#include "ac_save.h"
#include "ac_sample.h"


//CRC计算
const uint16 fcstab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
#define PPPINITFCS16 0xffff /* Initial FCS value */
#define PPPGOODFCS16 0xf0b8 /* Good final FCS value */

uint16 app_crc16_getex(uint16 CRC16_data,const uint8* buf, uint32 len)
{
    const unsigned char *cp=buf;
    while (len--)
        CRC16_data = (CRC16_data >> 8) ^ fcstab[(CRC16_data ^ *cp++) & 0xff];
    return (CRC16_data);
}
uint16 app_crc16_get(const uint8* buf, uint32 len)
{
    uint16 trialfcs;
    /* add on output */
    trialfcs = app_crc16_getex( PPPINITFCS16, buf, len );
    trialfcs ^= 0xffff; /* complement */
    return trialfcs;
}

/****************************************************************************
*模块编号：
*名称：
*功能：通过oad查询698转换内容
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
OOP_DAR_E get_convert_by_oad(uint8 *pDataBuffer, uint16 nDataLen, OAD_INFO_T *pOadInfo, RESULT_DATA_T *outData, uint16 outDataMax, BOOL bHasOad)
{
    ACUDP_FMT_TRACE("get_convert_by_oad() pOadInfo->pTab->oad.value=%08x, eData=%d, outDataMax=%d, bHasOad=%d, nDataLen=%d \n", pOadInfo->pTab->oad.value, pOadInfo->pTab->eData, outDataMax, bHasOad, nDataLen);

    uint32	nOAD = 0;
    int result = DATA_SUCCESS;

    uint32 outLen = 0;
    uint32 sourceoffset=0;
    uint32 desoffset=0;
    uint8 dstbuf[8192] = {0};
    DATA_CONVERT_T dsdata;
    uint32 offset = 0;

    //转换成报文数据
    dsdata.srcBuf = pDataBuffer;
    dsdata.srcLen = nDataLen;
    dsdata.sOffset = &sourceoffset;
    
    dsdata.dstBuf = dstbuf;
    dsdata.dstLen = outDataMax;
    dsdata.dOffset = &desoffset;

    nOAD = pOadInfo->pTab->oad.value;
    
    switch(nOAD)
    {
        /*********************** class1 ***********************/
        case 0x00100200:						//(当前)正向有功电能总尖峰平谷示值
        case 0x00200200:						//(当前)反向有功电能总尖峰平谷示值
        case 0x00300200:						//(当前)正向无功电能总尖峰平谷示值
        case 0x00400200:						//(当前)反向无功电能总尖峰平谷示值
        case 0x00500200:						//(当前)第一象限无功总尖峰平谷电能示值
        case 0x00600200:						//(当前)第二象限无功总尖峰平谷电能示值
        case 0x00700200:						//(当前)第三象限无功总尖峰平谷电能示值
        case 0x00800200:						//(当前)第四象限无功总尖峰平谷电能示值
        case 0x00110200:						//(A相)正向有功总电能示值
        case 0x00210200:						//(A相)反向有功总电能示值
        case 0x00310200:						//(A相)正向无功总电能示值
        case 0x00410200:						//(A相)反向无功总电能示值
        case 0x00120200:						//(B相)正向有功总电能示值
        case 0x00220200:						//(B相)反向有功总电能示值
        case 0x00320200:						//(B相)正向无功总电能示值
        case 0x00420200:						//(B相)反向无功总电能示值
        case 0x00130200:						//(C相)正向有功总电能示值
        case 0x00230200:						//(C相)反向有功总电能示值
        case 0x00330200:						//(C相)正向无功总电能示值
        case 0x00430200:						//(C相)反向无功总电能示值
        /* 增加 电能 start*/
        case 0x00000200:						//(当前)组合有功电能总尖峰平谷示值
        case 0x00900200:						//(当前)正向视在电能总尖峰平谷示值
        case 0x00A00200:						//(当前)反向视在电能总尖峰平谷示值
        case 0x00510200:						//A相第一象限无功电能
        case 0x00520200:						//B相第一象限无功电能
        case 0x00530200:						//C相第一象限无功电能
        case 0x00610200:						//A相第二象限无功电能
        case 0x00620200:						//B相第二象限无功电能
        case 0x00630200:						//C相第二象限无功电能
        case 0x00710200:						//A相第三象限无功电能
        case 0x00720200:						//B相第三象限无功电能
        case 0x00730200:						//C相第三象限无功电能
        case 0x00810200:						//A相第四象限无功电能
        case 0x00820200:						//B相第四象限无功电能
        case 0x00830200:						//C相第四象限无功电能
        case 0x00910200:						//A相正向视在电能
        case 0x00920200:						//B相正向视在电能
        case 0x00930200:						//C相正向视在电能
        case 0x00A10200:						//A相反向视在电能
        case 0x00A20200:						//B相反向视在电能
        case 0x00A30200:						//C相反向视在电能
        /* 增加 电能 end*/
        {
            ACUDP_FMT_TRACE("get_convert_by_oad() data_to_buf_class1() 组帧 \n");

            result = data_to_buf_class1(pOadInfo, &dsdata);
            outLen = *dsdata.dOffset;
            ACUDP_FMT_TRACE("转outData前: result=%d, outLen=%d, *dsdata.dOffset=%d, dsdata.dstBuf:\n", result, outLen, *dsdata.dOffset);
            ACUDP_BUF_TRACE(dsdata.dstBuf, *dsdata.dOffset, "dsdata.dstBuf:");
            if((result == ERR_OK) && (outLen > 0))
            {
                if(bHasOad)
                {
                    memcpy_r(&outData->pResultData[offset], (uint8*)&pOadInfo->pTab->oad.value, 4);
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset] = 0x01;
                    offset ++;
                }
                memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
                outData->resultDataLen = offset + outLen;
                result = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
            }
        }
            break;

        
        /*********************** class2 ***********************/
        case 0x10100200:				//(当前)正向有功总尖峰平谷最大需量
        case 0x10200200:				//(当前)反向有功总尖峰平谷最大需量
        case 0x10300200:				//(当前)正向无功总尖峰平谷最大需量
        case 0x10400200:				//(当前)反向无功总尖峰平谷最大需量
        /* 增加 需量 begin*/
        case 0x10500200:				//(当前)1象限无功总尖峰平谷最大需量
        case 0x10600200:				//(当前)2象限无功总尖峰平谷最大需量
        case 0x10700200:				//(当前)3象限无功总尖峰平谷最大需量
        case 0x10800200:				//(当前)4象限无功总尖峰平谷最大需量
        case 0x10900200:				//(当前)正向视在电能总尖峰平谷最大需量
        case 0x10A00200:				//(当前)反向视在电能总尖峰平谷最大需量
        /* 增加 需量 end*/
        /* 增加 分项需量 begin*/
        case 0x10110200:				//(当前)A相 正向有功最大需量
        case 0x10120200:				//(当前)B相 正向有功最大需量
        case 0x10130200:				//(当前)C相 正向有功最大需量
        case 0x10210200:				//(当前)A相 反向有功最大需量
        case 0x10220200:				//(当前)B相 反向有功最大需量
        case 0x10230200:				//(当前)C相 反向有功最大需量
        case 0x10310200:				//(当前)A相 组合无功1最大需量
        case 0x10320200:				//(当前)B相 组合无功1最大需量
        case 0x10330200:				//(当前)C相 组合无功1最大需量
        case 0x10410200:				//(当前)A相 组合无功2最大需量
        case 0x10420200:				//(当前)B相 组合无功2最大需量
        case 0x10430200:				//(当前)C相 组合无功2最大需量
        case 0x10510200:				//(当前)A相 第一象限最大需量
        case 0x10520200:				//(当前)B相 第一象限最大需量
        case 0x10530200:				//(当前)C相 第一象限最大需量
        case 0x10610200:				//(当前)A相 第二象限最大需量
        case 0x10620200:				//(当前)B相 第二象限最大需量
        case 0x10630200:				//(当前)C相 第二象限最大需量
        case 0x10710200:				//(当前)A相 第三象限最大需量
        case 0x10720200:				//(当前)B相 第三象限最大需量
        case 0x10730200:				//(当前)C相 第三象限最大需量
        case 0x10810200:				//(当前)A相 第四象限最大需量
        case 0x10820200:				//(当前)B相 第四象限最大需量
        case 0x10830200:				//(当前)C相 第四象限最大需量
        case 0x10910200:				//(当前)A相 正向视在最大需量
        case 0x10920200:				//(当前)B相 正向视在最大需量
        case 0x10930200:				//(当前)C相 正向视在最大需量
        case 0x10A10200:				//(当前)A相 反向视在最大需量
        case 0x10A20200:				//(当前)B相 反向视在最大需量
        case 0x10A30200:				//(当前)C相 反向视在最大需量
        /* 增加 分项需量 end*/
        {
            ACUDP_FMT_TRACE("get_convert_by_oad() data_to_buf_class2() 组帧 \n");

            result = data_to_buf_class2(pOadInfo, &dsdata);
            outLen = *dsdata.dOffset;
            ACUDP_FMT_TRACE("转outData前: result=%d, outLen=%d, *dsdata.dOffset=%d, dsdata.dstBuf:\n", result, outLen, *dsdata.dOffset);
            ACUDP_BUF_TRACE(dsdata.dstBuf, *dsdata.dOffset, "dsdata.dstBuf:");
            if((result == ERR_OK) && (outLen > 0))
            {  	
                if(bHasOad)
                {
                    memcpy_r(&outData->pResultData[offset], (uint8*)&pOadInfo->pTab->oad.value, 4);
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset] = 0x01;
                    offset ++;
                }
                memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
                outData->resultDataLen = offset + outLen;
                result = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
            }
        }
            break;


        /*********************** class3 ***********************/
        case 0x20000200:			   //(当前)三相电压
        case 0x20010200:			   //(当前)三相电流
        /* 增加	零序电压/电流 begin*/
        case 0x20000600:			   //(当前)零序电压
        case 0x20010600:			   //(当前)零序电流
        /* 增加	零序电压/电流 end*/
        /* 增加	相角 begin*/
        case 0x20020200:			   //(当前)电压相角
        case 0x20030200:			   //(当前)电压电流相角
        /* 增加	相角 end*/
        case 0x20000201:			   //(当前)三相电压 A分项
        case 0x20000202:			   //(当前)三相电压 B分项
        case 0x20000203:			   //(当前)三相电压 C分项
        case 0x20010201:			   //(当前)三相电流 A分项
        case 0x20010202:			   //(当前)三相电流 B分项
        case 0x20010203:			   //(当前)三相电流 C分项
        {
            ACUDP_FMT_TRACE("get_convert_by_oad() data_to_buf_class3() 组帧 \n");

            result = data_to_buf_class3(pOadInfo, &dsdata);
            outLen = *dsdata.dOffset;
            ACUDP_FMT_TRACE("转outData前: result=%d, outLen=%d, *dsdata.dOffset=%d, dsdata.dstBuf:\n", result, outLen, *dsdata.dOffset);
            ACUDP_BUF_TRACE(dsdata.dstBuf, *dsdata.dOffset, "dsdata.dstBuf:");
            if((result == ERR_OK) && (outLen > 0))
            {  	
               if(bHasOad)
                {
                    memcpy_r(&outData->pResultData[offset], (uint8*)&pOadInfo->pTab->oad.value, 4);
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset] = 0x01;
                    offset ++;
                }
                memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
                outData->resultDataLen = offset + outLen;
                result = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
            }
        }
            break;
        
            
        /*********************** class4 ***********************/
        case 0x20040200:						//(当前)瞬时有功功率
        case 0x20050200:			            //(当前)瞬时无功功率
        /* 增加	视在功率 begin*/
        case 0x20060200:						//(当前)瞬时视在功率
        /* 增加	视在功率 end*/
        case 0x200A0200:						//(当前)瞬时总及分相功率因数

        case 0x200A0201:						//(当前)瞬时总及分相功率因数 总
        case 0x200A0202:						//(当前)瞬时总及分相功率因数 A
        case 0x200A0203:						//(当前)瞬时总及分相功率因数 B
        case 0x200A0204:						//(当前)瞬时总及分相功率因数 C
        {
            ACUDP_FMT_TRACE("get_convert_by_oad() data_to_buf_class4() 组帧 \n");

            result = data_to_buf_class4(pOadInfo, &dsdata);
            outLen = *dsdata.dOffset;
            ACUDP_FMT_TRACE("转outData前: result=%d, outLen=%d, *dsdata.dOffset=%d, dsdata.dstBuf:\n", result, outLen, *dsdata.dOffset);
            ACUDP_BUF_TRACE(dsdata.dstBuf, *dsdata.dOffset, "dsdata.dstBuf:");
            if((result == ERR_OK) && (outLen > 0))
            {  	
                if(bHasOad)
                {
                    memcpy_r(&outData->pResultData[offset], (uint8*)&pOadInfo->pTab->oad.value, 4);
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset] = 0x01;
                    offset ++;
                }
                memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
                outData->resultDataLen = offset + outLen;
                result = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
            }
        }
            break;
        

        /*********************** class6 ***********************/
        case 0x200F0200:						//(当前)电网频率
        /* 增加	电表状态字1-7 begin*/
        case 0x20140200:						//电表运行状态字1-7
        /* 增加	电表状态字1-7 end*/
        /***增加 电压不平衡率 begin***/
        case 0x20260200:						//电压不平衡率
        /***增加 电压不平衡率 end***/
        /***增加 电流不平衡率 begin***/
        case 0x20270200:						//电流不平衡率
        /***增加 电流不平衡率 end***/
        {
            ACUDP_FMT_TRACE("get_convert_by_oad() data_to_buf_class6() 组帧 \n");

            result = data_to_buf_class6(pOadInfo, &dsdata);
            outLen = *dsdata.dOffset;
            ACUDP_FMT_TRACE("转outData前: result=%d, outLen=%d, *dsdata.dOffset=%d, dsdata.dstBuf:\n", result, outLen, *dsdata.dOffset);
            ACUDP_BUF_TRACE(dsdata.dstBuf, *dsdata.dOffset, "dsdata.dstBuf:");
            if((result == ERR_OK) && (outLen > 0))
            {  	
                if(bHasOad)
                {
                    memcpy_r(&outData->pResultData[offset], (uint8*)&pOadInfo->pTab->oad.value, 4);
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset] = 0x01;
                    offset ++;
                }
                memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
                outData->resultDataLen = offset + outLen;
                result = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
            }
        }
            break;
        

        /*********************** class5 ***********************/
        /* 增加 谐波 begin*/
        case 0x200D0200:			   //(当前)A相 电压谐波含有率（总及2…n次）
        case 0x200D0300:			   //(当前)B相 电压谐波含有率（总及2…n次）
        case 0x200D0400:			   //(当前)C相 电压谐波含有率（总及2…n次）
        case 0x200E0200:			   //(当前)A相 电流谐波含有率（总及2…n次）
        case 0x200E0300:			   //(当前)B相 电流谐波含有率（总及2…n次）
        case 0x200E0400:			   //(当前)C相 电流谐波含有率（总及2…n次）
        /* 增加 谐波 end*/
        /* 增加 谐波含量 begin*/
        case 0x20330200:			   //(当前)A相 电压谐波含量（总及2…n次）
        case 0x20330300:			   //(当前)B相 电压谐波含量（总及2…n次）
        case 0x20330400:			   //(当前)C相 电压谐波含量（总及2…n次）
        case 0x20340200:			   //(当前)A相 电流谐波含量（总及2…n次）
        case 0x20340300:			   //(当前)B相 电流谐波含量（总及2…n次）
        case 0x20340400:			   //(当前)C相 电流谐波含量（总及2…n次）
        /* 增加 谐波含量 end*/
        {
            ACUDP_FMT_TRACE("get_convert_by_oad() data_to_buf_class5() 组帧 \n");

            result = data_to_buf_class5(pOadInfo, &dsdata);
            outLen = *dsdata.dOffset;
            ACUDP_FMT_TRACE("转outData前: result=%d, outLen=%d, *dsdata.dOffset=%d, dsdata.dstBuf:\n", result, outLen, *dsdata.dOffset);
            ACUDP_BUF_TRACE(dsdata.dstBuf, *dsdata.dOffset, "dsdata.dstBuf:");
            if((result == ERR_OK) && (outLen > 0))
            {  	
                if(bHasOad)
                {
                    memcpy_r(&outData->pResultData[offset], (uint8*)&pOadInfo->pTab->oad.value, 4);
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset] = 0x01;
                    offset ++;
                }
                memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
                outData->resultDataLen = offset + outLen;
                result = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
            }
        }
            break;

        /*********************** class8 ***********************/
        case 0x40000200:			   //交采时间
        case 0x40080200:			   //备用套时区表切换时间
        case 0x40090200:			   //备用套日时段切换时间
        case 0x400C0200:			   //时区时段数
        case 0x40140200:			   //当前套时区表
        case 0x40150200:			   //备用套时区表
        case 0x40160200:			   //当前套日时段表
        case 0x40170200:			   //备用套日时段表
        case 0x41000200:			   //最大需量周期
        case 0x41010200:			   //滑差时间
        case 0x41120200:			   //有功组合方式特征字
        case 0x41130200:			   //无功组合方式1特征字
        case 0x41140200:			   //无功组合方式2特征字

        case 0x40160201:               //当前套日时段表 第1套
        case 0x40160202:               //当前套日时段表 第2套
        case 0x40160203:               //当前套日时段表 第3套
        case 0x40160204:               //当前套日时段表 第4套
        case 0x40160205:               //当前套日时段表 第5套
        case 0x40160206:               //当前套日时段表 第6套
        case 0x40160207:               //当前套日时段表 第7套
        case 0x40160208:               //当前套日时段表 第8套
        {
            ACUDP_FMT_TRACE("get_convert_by_oad() data_to_buf_class8() 组帧 \n");

            result = data_to_buf_class8(pOadInfo, &dsdata);
            outLen = *dsdata.dOffset;
            ACUDP_FMT_TRACE("转outData前: result=%d, outLen=%d, *dsdata.dOffset=%d, dsdata.dstBuf:\n", result, outLen, *dsdata.dOffset);
            ACUDP_BUF_TRACE(dsdata.dstBuf, *dsdata.dOffset, "dsdata.dstBuf:");
            if((result == ERR_OK) && (outLen > 0))
            {  	
                if(bHasOad)
                {
                    memcpy_r(&outData->pResultData[offset], (uint8*)&pOadInfo->pTab->oad.value, 4);
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset] = 0x01;
                    offset ++;
                }
                memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
                outData->resultDataLen = offset + outLen;
                result = DATA_SUCCESS;
            }
            else
            {
                result = DATA_READ_WRITE_DENIED;
            }
        }
            break;
        case 0x43000300:
        {
            result = data_to_buf_class19(pOadInfo, &dsdata);
            outLen = *dsdata.dOffset;
            if ((result == ERR_OK) && (outLen > 0))
            {
                if (bHasOad)
                {
                    memcpy_r(&outData->pResultData[offset], (uint8*)&pOadInfo->pTab->oad.value, 4);
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset] = 0x01;
                    offset++;
                }
                memcpy(&outData->pResultData[offset], dsdata.dstBuf, outLen);
                outData->resultDataLen = offset + outLen;
                result = DATA_SUCCESS;
            }
            else
            {
                ACUDP_FMT_DEBUG("data_to_buf_class19 err. oad[0x%08x] result[%d] outLen[%d]\n", pOadInfo->pTab->oad.value, result, outLen);
                ACUDP_BUF_DEBUG(dsdata.srcBuf, dsdata.srcLen, "srcdata:");
                result = DATA_READ_WRITE_DENIED;
            }
            break;
        }

        default:
            return -1;
            break;
   }

   ACUDP_FMT_TRACE("转outData后: result[%d], oad[0x%08x], resultDataLen[%d]\n", result, pOadInfo->pTab->oad.value, outData->resultDataLen);
   ACUDP_BUF_TRACE(outData->pResultData, outData->resultDataLen, "pResultData:");

   return result;
}



/**
*********************************************************************
* @name：       class_oadinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
    uint8 OIA  = 0;
    uint8 OIA1 = 0;
    uint8 OIA2 = 0;
    OIA = (uint8)(oad.nObjID>>8);
    OIA1 = (uint8)((OIA>>4)&0x0f);
    OIA2 = (uint8)((OIA)&0x0f);
    OOP_OAD_U blockOAD = {0};
    uint32 size = 0;
    const OAD_TAB_T *pstOdaTab = NULL;

    //不为0才赋值，否则递归调用后会被清零
    if(oad.nIndex != 0)  
    {
        pstOdaInfo->nIndex = oad.nIndex;
    }

    switch(OIA1)
    {
        case 0x00: pstOdaTab = oadEnergyInfoList; size = oadEnergyInfoListLen; break;
        case 0x01: pstOdaTab = oadDemandInfoList; size = oadDemandInfoListLen; break; 
        case 0x02: pstOdaTab = oadVariateInfoList; size = oadVariateInfoListLen; break;
        case 0x03: pstOdaTab = oadEvnetInfoList; size = oadEvnetInfoListLen; break;
        case 0x04: pstOdaTab = oadParaInfoList; size = oadParaInfoListLen; break;
        case 0x05: pstOdaTab = oadFreezeInfoList; size = oadFreezeInfoListLen; break;
        default:break;
    }

    if(pstOdaTab != NULL)
    {
        while(size--)
        {
            if(pstOdaTab->oad.value == oad.value)
            {
                pstOdaInfo->pTab = pstOdaTab;
                pstOdaInfo->subeData = pstOdaTab->eData;
                return TRUE;
            }
            else if((pstOdaTab->oad.nObjID == oad.nObjID) &&
                    (pstOdaTab->oad.attID == oad.attID) &&
                    (pstOdaTab->oad.nIndex == oad.nIndex)) 
            {
                ACUDP_FMT_DEBUG("non-strict search oadInfoList. OAD(0x%08x), ATTPRO(%d), OIA1(%d), OIA2(%d)\n", oad.value, oad.attPro, OIA1, OIA2);
                pstOdaInfo->pTab = pstOdaTab;
                pstOdaInfo->subeData = pstOdaTab->eData;
                return TRUE;                
            }
                    
            pstOdaTab++;
        }
    }

    //如果索引不为0，查不中，再用0查一次
    if(oad.nIndex != 0)  
    {
        blockOAD.value = oad.value;
        blockOAD.nIndex = 0;
        
        ACUDP_FMT_TRACE("block search oadInfoList. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);
        return class_oadinfo_get(blockOAD, pstOdaInfo);
    }

    return FALSE;
}

/**
*********************************************************************
* @name：       class_oadinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *info     ODA信息
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int class_omdinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
    uint8 OIA  = 0;
    uint8 OIA1 = 0;
    //uint8 OIA2 = 0;
    OIA = (uint8)(oad.nObjID>>8);
    OIA1 = (uint8)((OIA>>4)&0x0f);
    //OIA2 = (uint8)((OIA)&0x0f);
    uint32 size = 0;
    const OMD_INFO_T *pstOdaTab = NULL;
     
    switch(OIA1)
    {
        case 0x05: pstOdaTab = omdFrozenInfoList; size = omdFrozenInfoListLen; break;
        default:break;
    }

    if(pstOdaTab != NULL)
    {
        while(size--)
        {
            if(pstOdaTab->omd.value == oad.value)
            {
                pstOdaInfo->pTab = (OAD_TAB_T *)pstOdaTab;
                return TRUE;
            }

            pstOdaTab++;
        }
    }

    return FALSE;
}


/*********************************************************************
 功能描述：更新一些需要特殊处理的数据
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
int special_data_update(uint32 OAD)
{
    int     i    = 0;
    time_t nStartTime = time(NULL);
    BOOL bUnfinished = FALSE;
    
    switch(OAD)
    {
        case 0x40160200:			   //当前套日时段表
        case 0x40160201:
        case 0x40160202:
        case 0x40160203:
        case 0x40160204:
        case 0x40160205:
        case 0x40160206:
        case 0x40160207:
        case 0x40160208:
        {
            ACUDP_FMT_TRACE("special_data_update() OAD=%08x 查询当前套日时段表\n", OAD);
            //首先查询当前套是第几套
            g_nReadCurrDayPeriLabelFlag = 1;
            g_nRefreshCurrDayPeriLabelFlag = 0;
            while(abs(time(NULL) - nStartTime) < 3)//超时时间
            {
                if( (0 == g_nReadCurrDayPeriLabelFlag)
                    && (1 == g_nRefreshCurrDayPeriLabelFlag) )
                {
                    ACUDP_FMT_TRACE("TermSharedData.g_nCurrDayPeriLabel=%d \n", TermSharedData.g_nCurrDayPeriLabel);
                    break;
                }
                usleep(50000);
            }

            //接着查询当前套信息
            nStartTime = time(NULL);
            if(0 == TermSharedData.g_nCurrDayPeriLabel)//当前套是第一套
            {
                g_nAcReadCurrDayPeriFlag = 1;
                memset(g_nAcReadCurrDayFlagArr, 0x01, TermSharedData.CurrDayPeriTableRD.nNum);
                memset(g_nRefreshCurrDayFlagArr, 0x00, TermSharedData.CurrDayPeriTableRD.nNum);

                while(abs(time(NULL) - nStartTime) < 3)//超时时间
                {
                    bUnfinished = FALSE;

                    for(i=0; i<TermSharedData.CurrDayPeriTableRD.nNum; i++)
                    {
                        if( (1 == g_nAcReadCurrDayPeriFlag
                            || 1 == g_nAcReadCurrDayFlagArr[i]
                            || 0 == g_nRefreshCurrDayFlagArr[i]) )
                        {
                            bUnfinished = TRUE;
                            break;
                        }
                    }
                    
                    if(!bUnfinished)
                    {
                        ACUDP_FMT_TRACE("当前套第1套, 第1套已更新 \n");
                        break;
                    }
                    usleep(50000);
                }
            }
            else if(1 == TermSharedData.g_nCurrDayPeriLabel)//当前套是第二套
            {
                g_nAcReadAddiDayPeriFlag = 1;
                memset(g_nAcReadAddiDayFlagArr, 0x01, TermSharedData.AddiDayPeriTableRD.nNum);
                memset(g_nRefreshAddiDayFlagArr, 0x00, TermSharedData.AddiDayPeriTableRD.nNum);

                while(abs(time(NULL) - nStartTime) < 3)//超时时间
                {
                    bUnfinished = FALSE;

                    for(i=0; i<TermSharedData.AddiDayPeriTableRD.nNum; i++)
                    {
                        if( (1 == g_nAcReadAddiDayPeriFlag
                            || 1 == g_nAcReadAddiDayFlagArr[i]
                            || 0 == g_nRefreshAddiDayFlagArr[i]) )
                        {
                            bUnfinished = TRUE;
                            break;
                        }
                    }
                    
                    if(!bUnfinished)
                    {
                        ACUDP_FMT_TRACE("当前套第2套, 第2套已更新 \n");
                        break;
                    }
                    usleep(50000);
                }
            }
        }
            break;
        case 0x40170200:			   //备用套日时段表
        case 0x40170201:
        case 0x40170202:
        case 0x40170203:
        case 0x40170204:
        case 0x40170205:
        case 0x40170206:
        case 0x40170207:
        case 0x40170208:
        {
            ACUDP_FMT_TRACE("special_data_update() OAD=%08x 查询备用套日时段表\n", OAD);
            //首先查询当前套是第几套
            g_nReadCurrDayPeriLabelFlag = 1;
            g_nRefreshCurrDayPeriLabelFlag = 0;
            while(abs(time(NULL) - nStartTime) < 3)//超时时间
            {
                if( (0 == g_nReadCurrDayPeriLabelFlag)
                    && (1 == g_nRefreshCurrDayPeriLabelFlag) )
                {
                    ACUDP_FMT_TRACE("TermSharedData.g_nCurrDayPeriLabel=%d \n", TermSharedData.g_nCurrDayPeriLabel);
                    break;
                }
                usleep(50000);
            }

            //接着查询当前套信息
            nStartTime = time(NULL);
            if(1 == TermSharedData.g_nCurrDayPeriLabel)//当前套是第二套
            {
                g_nAcReadCurrDayPeriFlag = 1;
                memset(g_nAcReadCurrDayFlagArr, 0x01, TermSharedData.CurrDayPeriTableRD.nNum);
                memset(g_nRefreshCurrDayFlagArr, 0x00, TermSharedData.CurrDayPeriTableRD.nNum);

                while(abs(time(NULL) - nStartTime) < 3)//超时时间
                {
                    bUnfinished = FALSE;

                    for(i=0; i<TermSharedData.CurrDayPeriTableRD.nNum; i++)
                    {
                        if( (1 == g_nAcReadCurrDayPeriFlag
                            || 1 == g_nAcReadCurrDayFlagArr[i]
                            || 0 == g_nRefreshCurrDayFlagArr[i]) )
                        {
                            bUnfinished = TRUE;
                            break;
                        }
                    }
                    
                    if(!bUnfinished)
                    {
                        ACUDP_FMT_TRACE("当前套第2套, 第1套已更新 \n");
                        break;
                    }
                    usleep(50000);
                }
            }
            else if(0 == TermSharedData.g_nCurrDayPeriLabel)//当前套是第一套
            {
                g_nAcReadAddiDayPeriFlag = 1;
                memset(g_nAcReadAddiDayFlagArr, 0x01, TermSharedData.AddiDayPeriTableRD.nNum);
                memset(g_nRefreshAddiDayFlagArr, 0x00, TermSharedData.AddiDayPeriTableRD.nNum);

                while(abs(time(NULL) - nStartTime) < 3)//超时时间
                {
                    bUnfinished = FALSE;

                    for(i=0; i<TermSharedData.AddiDayPeriTableRD.nNum; i++)
                    {
                        if( (1 == g_nAcReadAddiDayPeriFlag
                            || 1 == g_nAcReadAddiDayFlagArr[i]
                            || 0 == g_nRefreshAddiDayFlagArr[i]) )
                        {
                            bUnfinished = TRUE;
                            break;
                        }
                    }
                    
                    if(!bUnfinished)
                    {
                        ACUDP_FMT_TRACE("当前套第1套, 第2套已更新 \n");
                        break;
                    }
                    usleep(50000);
                }
            }
        }
            break;

        case 0x43000300:
        {
            ACUDP_FMT_DEBUG("开始重新获取版本信息\n");
            ac_clean_ver();
            while (abs(time(NULL) - nStartTime) < 4)//超时时间
            {
                if (!isJCNeedGetVer())
                {
                    break;
                }
                usleep(50000);
            }
            if (isJCNeedGetVer())
            {
                ACUDP_FMT_DEBUG("获取版本信息失败或者超时\n");
            }
            else
            {
                ACUDP_FMT_DEBUG("获取版本信息成功\n");
            }
            break;
        }   

        default:
            return -1;
            break;
    }

    return 1;
}

/*********************************************************************
 功能描述：从内存中获取数据(即附录中的数据项)
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
int get_data_from_mem(uint8 *pDesbuffer, uint16 *nDesLen, uint32 OAD)
{
    const Map645ToOop_NEW *pMap645ToOop = NULL;
    int ret  = 0;
    
    //查找645与oad匹配关系
    pMap645ToOop = FindDetailInfoByOad(OAD);
    if(pMap645ToOop == NULL)
    {
        ACUDP_FMT_TRACE("FindDetailInfoByOad() if(pMap645ToOop == NULL), OAD=%08x \n", OAD);
        return -1;
    }

    //查找交采数据结构体
    ret = Find07AcDataByOad(pDesbuffer, nDesLen, pMap645ToOop);
    if(ret < 0)
    {
        ACUDP_FMT_TRACE("Find07AcDataByOad() if(ret < 0) ret=%d, OAD=%08x \n", ret, OAD);
        return -2;
    }

    return 1;
}

/*********************************************************************
 功能描述：从数据中心获取数据(非附录中的数据项)
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
int get_data_from_db(uint8 *pDesbuffer, uint16 *nDesLen, uint32 OAD)
{
    switch(OAD)
    {
        /*class 3*/
        case 0x20000201:               //交采A相电压
        case 0x20000202:               //交采B相电压
        case 0x20000203:               //交采C相电压
        case 0x20010201:               //交采A相电流
        case 0x20010202:               //交采B相电流
        case 0x20010203:               //交采C相电流
        {
            combine_sub_data_class3_buff(pDesbuffer, nDesLen, OAD);
        }
            break;
        
        /*class 4*/
        case 0x200a0201:               //交采总功率因数
        case 0x200a0202:               //交采A相总功率因数
        case 0x200a0203:               //交采B相总功率因数
        case 0x200a0204:               //交采C相总功率因数
        {
            combine_sub_data_class4_buff(pDesbuffer, nDesLen, OAD);
        }
            break;
        
        /*class 8*/
        case 0x40000200:               //交采时钟
        {
            combine_time_normal_buff(pDesbuffer, nDesLen);
        }
            break;
        case 0x40160201:               //当前套日时段表
        case 0x40160202:               //当前套日时段表
        case 0x40160203:               //当前套日时段表
        case 0x40160204:               //当前套日时段表
        case 0x40160205:               //当前套日时段表
        case 0x40160206:               //当前套日时段表
        case 0x40160207:               //当前套日时段表
        case 0x40160208:               //当前套日时段表
        {
            combine_sub_data_class8_buff(pDesbuffer, nDesLen, OAD);
        }
            break;

        default:
            return -1;
            break;
    }

    return 1;
}


/*********************************************************************
 功能描述：处理单个oad的组帧
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 get_single_oad_buff(OOP_OAD_U *serviceOAD, RESULT_DATA_T *outData, uint16 msgmaxlen, BOOL bHasOad, int datatype)
{
    uint8   aStructBuff[MAX_DB_R_W_LEN] = {0};
    uint16  nStructBuffLen = 0;
    int     ret  = 0;
    OAD_INFO_T pstOadInfo = {0};
    memset(aStructBuff, 0x00, sizeof(aStructBuff));

    ACUDP_FMT_TRACE("get_single_oad_buff() serviceOAD->value=%08x, msgmaxlen=%d, bHasOad=%d, datatype=%d \n", serviceOAD->value, msgmaxlen, bHasOad, datatype);
    
    if (class_oadinfo_get(*serviceOAD, &pstOadInfo))
    {
        ACUDP_FMT_TRACE("get_single_oad_buff() pstOadInfo.pTab->oad.value=%08x, class=%d, OIA1=%d, OIA2=%d, dataType=%d, dataLen=%d, eData=%d \n", 
            pstOadInfo.pTab->oad.value, pstOadInfo.pTab->class, 
            pstOadInfo.pTab->OIA1, pstOadInfo.pTab->OIA2, pstOadInfo.pTab->dataType, pstOadInfo.pTab->dataLen, pstOadInfo.pTab->eData);

        //特殊数据需要先从交采模块获取更新
        special_data_update(serviceOAD->value);

        //先从内存中查询数据
        ret = get_data_from_mem(aStructBuff, &nStructBuffLen, serviceOAD->value);
        if(ret < 0)
        {
            //再从数据中心查询数据
            ret = get_data_from_db(aStructBuff, &nStructBuffLen, serviceOAD->value);
            if(ret < 0)
            {
                return 0;
            }
        }

        //将交采数据转化为698对应格式
        ACUDP_FMT_TRACE("get_single_oad_buff() serviceOAD->value=%08x, 将交采数据转化为698对应格式 nStructBuffLen=%d \n", serviceOAD->value, nStructBuffLen);
        ACUDP_BUF_TRACE(aStructBuff, nStructBuffLen, "结构体aStructBuff:");
        ret = get_convert_by_oad(aStructBuff, nStructBuffLen, &pstOadInfo, outData, msgmaxlen, bHasOad);
        if(ret != DATA_SUCCESS)
        {
            ACUDP_FMT_TRACE("get_single_oad_buff() get_convert_by_oad() if(ret != DATA_SUCCESS) ret=%d, serviceOAD->value=%08x \n", ret, serviceOAD->value);
            return 0;
        }

    }
    else
    {
        ACUDP_FMT_TRACE("get_single_oad_buff() class_oadinfo_get(*serviceOAD, &pstOadInfo) 失败! \n");
    }

    return outData->resultDataLen;
}

/*********************************************************************
 功能描述：检查oad是否支持
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
int check_oad_is_valid(OOP_OAD_U *oad, uint8 servicetype)
{
    int result = -1;
    int i = 0;

    if(servicetype == SERVERID_GETREQUEST)//读取请求
    {
        for(i=0; i<gSaveTaskAllNum; i++)
        {
            if(oad->value == gSaveTaskAll[i])
            {
                result = AC_NORMAL_DATA;//普通数据(实时、电量、需量、谐波)
                break;
            }
        }

        if (result < 0)
        {
            for(i=0; i<gSubOadDataNum; i++)
            {
                if(oad->value == gSubOadData[i])
                {
                    result = AC_NORMAL_DATA;//普通数据(实时、电量、需量、谐波)
                    break;
                }
            }
        }

        if (result < 0)
        {
            for(i=0; i<gRecordDataNum; i++)
            {
                if(oad->value == gRecordData[i])
                {
                    result = AC_RECORD_DATA;//记录型数据(事件)
                    break;
                }
            }
        }

        if (result < 0)
        {
            for (i = 0; i < gSaveTaskParaNum; i++)
            {
                if (oad->value == gSaveTaskPara[i])
                {
                    result = AC_PARA_DATA;//参数数据
                    break;
                }
            }
        }
    }
    else if(servicetype == SERVERID_SETREQUEST)//设置请求
    {
        for(i=0; i<gSaveTaskParaNum; i++)
        {
            if(oad->value == gSaveTaskPara[i])
            {
                result = AC_PARA_DATA;//参数数据
                break;
            }
        }
    }
    else if(servicetype == SERVERID_ACTIONREQUEST)//操作请求
    {
        for(i=0; i<gActionParaNum; i++)
        {
            if(oad->value == gActionPara[i])
            {
                result = AC_ACTION_DATA;//操作类型
                break;
            }
        }
    }

    return result;
}

/**********************************************************************
* @name      : app_frame_to_package
* @brief     ：组装报文, 帧头大小不需要准确填写(不支持长度扩展)
* @param[in] ：FRAME_HEAD_T *pFrame           帧头信息
               uint16 bufLen                  buf大小
               BOOLEAN bSC                       是否处理扰码
* @param[out]：uint8 *pBuf                    报文缓存
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
uint16 app_frame_to_package(FRAME_HEAD_T *pFrame, uint8 *pBuf, uint16 bufLen, BOOLEAN bSC)
{
    uint8 *temp = NULL;
    uint16 msgLen = 0;
    uint16 CRC = 0;
    uint16 i;

    if(NULL == pFrame || TRUE == pFrame->bExtmsg || NULL == pBuf || pFrame->headlen + pFrame->apduLen + 4 > bufLen 
       || pFrame->headlen < APP_698_HEAD_MIN_LEN)
    {
        return 0;
    }
    temp = pBuf;

    /* 起始字符 */
    *temp = 0x68;
    temp++;

    /* 长度域L */
    *temp = pFrame->msglen & 0xff;
    *(temp+1) = (pFrame->msglen & 0x3f00) >> 8;
    temp += APP_698_LEN_REGIN_SIZE;

    /* 控制域C */
    *temp = (pFrame->funCode & 0x07) + (pFrame->sc << 3) + (pFrame->separate << 5) +\
            (pFrame->prm << 6) + (pFrame->dir << 7);
    temp += APP_698_CTL_REGIN_SIZE;

    /* SA */
    if(pFrame->sAddr.logicAddr > 1)
    {
        /* 扩展逻辑地址 */
        *temp = (pFrame->sAddr.len & 0x0f) + (0x01 << 5) + ((pFrame->sAddr.type & 0x03) << 6);
        temp++;
        *temp = pFrame->sAddr.logicAddr;
    }
    else
    {
        *temp = ((pFrame->sAddr.len - 1) & 0x0f) + (pFrame->sAddr.logicAddr << 4) + ((pFrame->sAddr.type & 0x03) << 6);
    }
    temp++;
    appmsg_memcpy_r(temp, pFrame->sAddr.addr, pFrame->sAddr.len);
    temp += pFrame->sAddr.len;

    /* CA */
    *temp = pFrame->cAddr;
    temp += APP_698_CA_REGIN_SIZE;

    /* 先跳过HCS */
    msgLen = (uint8)(temp - pBuf - 1) + APP_698_CS_REGIN_SIZE;
    temp += APP_698_CS_REGIN_SIZE;
    pFrame->headlen = msgLen;

    /* 分帧格式 */
    if(pFrame->separate)
    {
        if(pFrame->headlen + 1 + APP_698_SEPARA_REGIN_SIZE > bufLen)
        {
            return 0;
        }
        *temp = pFrame->tSepara.block & 0xff;
        *(temp+1) = ((pFrame->tSepara.block >> 8) & 0x0f) + ((pFrame->tSepara.type << 6) & 0xc0);
        msgLen += APP_698_SEPARA_REGIN_SIZE;
        temp += APP_698_SEPARA_REGIN_SIZE;
    }
    
    if(1 + msgLen + pFrame->apduLen + APP_698_CS_REGIN_SIZE + 1 > bufLen)
    {
        return 0;
    }

    /* APDU内容 */
    if(0 != pFrame->apduLen && NULL != pFrame->pApdu)
    {
        memcpy(temp, pFrame->pApdu, pFrame->apduLen);
        temp += pFrame->apduLen;
        msgLen += pFrame->apduLen;
    }

    /* 扰码处理 */
    if(pFrame->sc && TRUE == bSC)
    {
        for(i = 1 + pFrame->headlen; i < 1 + msgLen; i++)
        {
            *(pBuf + i) += 0x33;
        }
    }

    /* 跳过FCS */
    temp += APP_698_CS_REGIN_SIZE;
    msgLen += APP_698_CS_REGIN_SIZE;

    /* 结束标记 */
    *temp = 0x16;

    /* 长度域回填 */
    pBuf[1] = msgLen & 0xff;
    pBuf[2] = (msgLen & 0x3f00) >> 8;
    
    /* 计算HCS */
    CRC = app_crc16_get(pBuf+1, pFrame->headlen - APP_698_CS_REGIN_SIZE);
    temp = pBuf + 1 + pFrame->headlen - APP_698_CS_REGIN_SIZE;
    *temp = CRC & 0xff;
    *(temp+1) = (CRC >> 8) & 0xff;

    /* 计算FCS */
    CRC = app_crc16_get(pBuf+1, msgLen - APP_698_CS_REGIN_SIZE);
    temp = pBuf + 1 + msgLen - APP_698_CS_REGIN_SIZE;
    *temp = CRC & 0xff;
    *(temp+1) = (CRC >> 8) & 0xff;

    pFrame->msglen = msgLen;
    
    return msgLen + 2;
}

/**********************************************************************
* @name      : app_check_message
* @brief     ：报文合法性检查
* @param[in] ：uint8 *pMsg           报文内容
               uint16 msgLen         报文长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
int app_check_message(uint8 *pMsg, uint32 msgLen)
{
    uint32 len = 0;
    uint8 *pTemp = NULL;
    uint16 CRC = 0;
    uint16 CS = 0;

    if(NULL == pMsg || msgLen < APP_698_MESSAGE_MIN_LEN)
    {
        return APP698_ERR_INPUT;
    }
    
    /* 1 检查首尾 */
    if(0x68 != *pMsg || 0x16 != pMsg[msgLen-1])
    {
        return APP698_ERR_MSG_HEAD_TAIL;
    }

    /* 2 检查长度 */
    if(pMsg[2] >= 0x40)
    {
        len = pMsg[2] & 0x3f;
        len = (len << 8) + pMsg[1];
        len = len * 1024 +2;
    }
    else
    {
        len = pMsg[1] + (((uint16)pMsg[2]) << 8) + 2;
    }
    if(len != msgLen)
    {
        return APP698_ERR_MSG_TOO_LONG;
    }

    /* 3 帧头校验 HCS */
    pTemp = pMsg + APP_698_A_REGIN_OFFSET;
    len = APP_698_LEN_REGIN_SIZE + APP_698_CTL_REGIN_SIZE + (*pTemp & 0x0f) + 2 + APP_698_CA_REGIN_SIZE; //帧长度
    CRC = app_crc16_get(pMsg + 1, len);
    pTemp = pMsg + 1 + len;
    CS = *pTemp + ((uint16)*(pTemp + 1)) * 0x100;
    if(CRC != CS)
    {
        return APP698_ERR_HCS;
    }

    /* 4 帧校验FCS */
    CRC = app_crc16_get(pMsg + 1, msgLen - 4);
    pTemp = pMsg + msgLen - 3;
    CS = *pTemp + ((uint16)*(pTemp + 1)) * 0x100;
    if(CRC != CS)
    {
        return APP698_ERR_FCS;
    }

    return 0;
}

/**********************************************************************
* @name      : app_get_frame
* @brief     ：解析报文帧头数据, 同时处理扰码
* @param[in] ：uint8 *pMsg           报文内容
               uint32 msgLen         报文长度
               BOOLEAN bSC           SC是否转换
* @param[out]：FRAME_HEAD_T *pFrame  帧头信息
* @return    ：错误码
* @Create    : 
* @Date      ：2019-12-4
* @Update    :
**********************************************************************/
int app_get_frame(uint8 *pMsg, uint32 msgLen, FRAME_HEAD_T *pFrame, BOOLEAN bSC)
{
    uint8 *temp = NULL;
    uint16 i;

    if(NULL == pMsg || msgLen < APP_698_MESSAGE_MIN_LEN || NULL == pFrame)
    {
        return APP698_ERR_INPUT;
    }

    /*长度*/
    temp = pMsg + 1;
    pFrame->msglen = *temp + (((uint16)temp[1]) << 8);
    if(temp[1] >= 0x40)
    {
        /* 扩展长度 */
        pFrame->msglen = (pFrame->msglen & 0x3fff) * 1024;
        pFrame->bExtmsg = 1;
    }
    else
    {
        pFrame->bExtmsg = 0;
    }
    
    temp += APP_698_LEN_REGIN_SIZE;

    /* 控制域 */
    pFrame->funCode = *temp & 0x07;
    pFrame->sc = (*temp >> 3) & 0x01;
    pFrame->separate = (*temp >> 5) & 0x01;
    pFrame->prm = (*temp >> 6) & 0x01;
    pFrame->dir = (*temp >> 7) & 0x01;
    temp += APP_698_CTL_REGIN_SIZE;

    /* SA */
    pFrame->sAddr.len = (*temp & 0x0f) + 1;
    pFrame->sAddr.logicAddr = (*temp >> 4) & 0x03;
    pFrame->sAddr.type = (*temp >> 6) & 0x03;
    /* 逻辑地址扩展 bit5=1 后面一个字节是扩展逻辑地址(2-255) */
    if(pFrame->sAddr.logicAddr > 1)
    {
        temp++;
        pFrame->sAddr.logicAddr = *temp;
        pFrame->sAddr.len -= 1;
    }
    temp++;
    appmsg_memcpy_r(pFrame->sAddr.addr, temp, pFrame->sAddr.len);
    temp += pFrame->sAddr.len;

    /* CA */
    pFrame->cAddr = *temp;
    temp++;

    /* 计算帧头长度 */
    temp += APP_698_CS_REGIN_SIZE;  //HCS
    pFrame->headlen = (uint8)(temp - pMsg) - 1;

    /* APDU */
    pFrame->pApdu = temp;
    pFrame->apduLen = pFrame->msglen - pFrame->headlen - 2;

    /* 扰码处理 */
    if(pFrame->sc && TRUE == bSC)
    {
        for(i = 0; i < pFrame->apduLen; i++)
        {
            pFrame->pApdu[i] -= 0x33;
        }
    }

    /* 分帧格式解析 */
    if(pFrame->separate)
    {
        pFrame->tSepara.block = *temp + (((uint16)(*(temp + 1)&0x0f)) << 8);
        pFrame->tSepara.type = (*(temp + 1)&0xf0) >> 6;
        pFrame->pApdu += 2;
        pFrame->apduLen -= 2;
    }

    return APP698_SECCESS;
}

/**********************************************************************
* @name      : app_check_rpt_message
* @brief     ：解析报文帧头数据, 判断是否是上报报文
* @param[in] ：uint8 *pMsg            报文内容
               uint32 msgLen         报文长度
* @param[out]：FRAME_HEAD_T *pFrame  帧头信息
* @return    ：错误码
* @Create    : 
* @Date      ：2019-12-4
* @Update    :
**********************************************************************/
int app_check_rpt_message(uint8 *pMsg, uint32 msgLen)
{
    int ret = -1;
    FRAME_HEAD_T frame;

    memset(&frame, 0, sizeof(frame));

    ret = app_check_message((uint8 *)pMsg, msgLen);
    if (ret != 0)
    {
        return ret;
    }

    ret = app_get_frame(pMsg, msgLen, &frame, FALSE);
    if (ret != 0)
    {
        return ret;
    }

    if ((frame.apduLen > 0) && (frame.pApdu != NULL) && frame.pApdu[0] == 0x88)
    {
        return ERR_OK;
    }
    else
    {
        return ERR_NORMAL;
    }
}

/*********************************************************************
 功能描述：处理get服务的oad
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 invoke_get_req(const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    uint16 msglen = 0;
    OOP_OAD_U serviceOAD = {0};
    RESULT_DATA_T outData;
    memset(&outData, 0x00, sizeof(RESULT_DATA_T));
    int dataType = -1;//普通数据1, 交采参数2
    OAD_INFO_T pstOadInfo = {0};
    int i = 0;
    uint16 index = 0;
    uint8 bufftmp[8192] = {0};
    OOP_OAD_U tmpOAD = {0};
    OOP_DAR_E oopDar = DATA_OTHER_REASON;


    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));
    ACUDP_FMT_TRACE("invoke_get_req() serviceOAD=%08x \n", serviceOAD.value);

    //检查oad是否支持
    dataType = check_oad_is_valid(&serviceOAD, SERVERID_GETREQUEST);
    if(dataType < 0)  //不支持oad
    {
        ACUDP_FMT_TRACE("invoke_get_req() if(check_oad_is_valid(&serviceOAD) < 0) 不支持! \n", serviceOAD.value);
        memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
        index += sizeof(OOP_OAD_U);
        bufftmp[index] = 0x00;  //错误信息DAR
        index ++;
        bufftmp[index] = 0x04;  //错误码:对象未定义
        index ++;

        if(index > 0)
        {
            msglen = index;
            if(msglen > msgmaxlen)
            {
                msglen = msgmaxlen;
            }
            memcpy(pOutMsg, &bufftmp[0], msglen);
        }
        ACUDP_FMT_TRACE("invoke_get_req() 直接赋值错误码(对象未定义) index=%d, msglen=%d, msgmaxlen=%d \n", index, msglen, msgmaxlen);
        
        return msglen;
    }
    if(class_oadinfo_get(serviceOAD, &pstOadInfo))
    {
        ACUDP_FMT_TRACE("set_single_oad_buff() pstOadInfo.pTab->oad.value=%08x, class=%d, OIA1=%d, OIA2=%d, dataType=%d, dataLen=%d, eData=%d \n", 
        pstOadInfo.pTab->oad.value, pstOadInfo.pTab->class, 
        pstOadInfo.pTab->OIA1, pstOadInfo.pTab->OIA2, pstOadInfo.pTab->dataType, pstOadInfo.pTab->dataLen, pstOadInfo.pTab->eData);

        switch(pstOadInfo.pTab->oad.value)
        {
            case 0x300F0900:   //设置事件有效标记
            {
                oopDar = class7_invoke_get(&pstOadInfo, pInMsg, inLen, &outData, msgmaxlen);
                memcpy(pOutMsg,outData.pResultData,outData.resultDataLen);

//                memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
//                index += sizeof(OOP_OAD_U);
//                bufftmp[index] = oopDar;  //(OAD+执行结果)
//                index ++;
//
//                if(index > 0)
//                {
//                    msglen = index;
//                    if(msglen > msgmaxlen)
//                    {
//                        msglen = msgmaxlen;
//                    }
//                    memcpy(pOutMsg, &bufftmp[0], msglen);
//                }
                
                ACUDP_FMT_TRACE("invoke_set_req() 返回执行结果 index=%d, msglen=%d, msgmaxlen=%d, oopDar=%d \n", index, msglen, msgmaxlen, oopDar);
                return outData.resultDataLen;
            }
                break;

            default:
                break;
        }

    }
    if(serviceOAD.value == 0xF2130201)  //电量
    {
        ACUDP_FMT_TRACE("invoke_get_req() if(serviceOAD.value == 0xF2130201)  //电量 \n");
        memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
        index += sizeof(OOP_OAD_U);
        bufftmp[index] = 0x01;  //data
        index ++;
        bufftmp[index] = 0x02;  //structure
        index ++;
        ChangeLenToBuff(gSaveTaskEnergyNum, &index, &bufftmp[index]);  //seq of
        
        for(i=0; i<gSaveTaskEnergyNum; i++)
        {
            tmpOAD.value = gSaveTaskEnergy[i];
            get_single_oad_buff(&tmpOAD, &outData, msgmaxlen, FALSE, AC_NORMAL_DATA);
            memcpy(&bufftmp[index], &outData.pResultData[0], outData.resultDataLen);
            index += outData.resultDataLen;
            memset(&outData, 0x00, sizeof(RESULT_DATA_T));
        }
    }
    else if(serviceOAD.value == 0xF2130202)  //需量
    {
        ACUDP_FMT_TRACE("invoke_get_req() if(serviceOAD.value == 0xF2130202)  //需量 \n");
        memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
        index += sizeof(OOP_OAD_U);
        bufftmp[index] = 0x01;  //data
        index ++;
        bufftmp[index] = 0x02;  //structure
        index ++;
        ChangeLenToBuff(gSaveTaskDemandNum, &index, &bufftmp[index]);  //seq of
        
        for(i=0; i<gSaveTaskDemandNum; i++)
        {
            tmpOAD.value = gSaveTaskDemand[i];
            get_single_oad_buff(&tmpOAD, &outData, msgmaxlen, FALSE, AC_NORMAL_DATA);
            memcpy(&bufftmp[index], &outData.pResultData[0], outData.resultDataLen);
            index += outData.resultDataLen;
            memset(&outData, 0x00, sizeof(RESULT_DATA_T));
        }
    }
    else if(serviceOAD.value == 0xF2130203)  //实时变量
    {
        ACUDP_FMT_TRACE("invoke_get_req() if(serviceOAD.value == 0xF2130203)  //实时变量 \n");
        memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
        index += sizeof(OOP_OAD_U);
        bufftmp[index] = 0x01;  //data
        index ++;
        bufftmp[index] = 0x02;  //structure
        index ++;
        ChangeLenToBuff(gSaveTaskRealtimeNum, &index, &bufftmp[index]);  //seq of
        
        for(i=0; i<gSaveTaskRealtimeNum; i++)
        {
            tmpOAD.value = gSaveTaskRealtime[i];
            get_single_oad_buff(&tmpOAD, &outData, msgmaxlen, FALSE, AC_NORMAL_DATA);
            memcpy(&bufftmp[index], &outData.pResultData[0], outData.resultDataLen);
            index += outData.resultDataLen;
            memset(&outData, 0x00, sizeof(RESULT_DATA_T));
        }
    }
    else if(serviceOAD.value == 0xF2130204)  //谐波
    {
        ACUDP_FMT_TRACE("invoke_get_req() if(serviceOAD.value == 0xF2130204)  //谐波 \n");
        memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
        index += sizeof(OOP_OAD_U);
        bufftmp[index] = 0x01;  //data
        index ++;
        bufftmp[index] = 0x02;  //structure
        index ++;
        ChangeLenToBuff(gSaveTaskHarmonicNum, &index, &bufftmp[index]);  //seq of
        
        for(i=0; i<gSaveTaskHarmonicNum; i++)
        {
            tmpOAD.value = gSaveTaskHarmonic[i];
            get_single_oad_buff(&tmpOAD, &outData, msgmaxlen, FALSE, AC_NORMAL_DATA);
            memcpy(&bufftmp[index], &outData.pResultData[0], outData.resultDataLen);
            index += outData.resultDataLen;
            memset(&outData, 0x00, sizeof(RESULT_DATA_T));
        }
    }
    else  //单oad数据
    {
        ACUDP_FMT_TRACE("invoke_get_req() 单oad数据 \n");
        get_single_oad_buff(&serviceOAD, &outData, msgmaxlen, TRUE, dataType);
        memcpy(&bufftmp[index], &outData.pResultData[0], outData.resultDataLen);
        index += outData.resultDataLen;
    }

    if(index > 0)
    {
        msglen = index;
        if(msglen > msgmaxlen)
        {
            msglen = msgmaxlen;
        }
        memcpy(pOutMsg, &bufftmp[0], msglen);
    }
    ACUDP_FMT_TRACE("invoke_get_req() index=%d, msglen=%d, msgmaxlen=%d \n", index, msglen, msgmaxlen);
    
    return msglen;
}

/*********************************************************************
 功能描述：读取若干对象属性
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 get_normal_list_req(uint8 *pApdu, uint16 nApduLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    /*SetRequestNormalList∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         若干个对象属性            SEQUENCE OF
         {
            一个对象属性描述符 OAD，
         }
    }*/

    ACUDP_FMT_TRACE("【start】 get_normal_list_req() 【开始】...\n");
    
    uint16 msglen = 0;
    uint8 sequenceNum;
    uint16  dataLen = 0;
    uint16  offset = 0;
    uint8 *outTmp = &pOutMsg[OOP_DATA_LIST_OFFSET];   //留出空间写OAD前面部分（ServiceID,responseID,PIID-ID, SEQUENCE OF）
    uint16 msgmaxlenTmp = msgmaxlen-OOP_DATA_LIST_OFFSET; //留出空间写OAD前面部分
    uint16 tmpMsgLen = 0;

    sequenceNum = pApdu[3]; //序列个数
    
    while(sequenceNum--)
    {
        dataLen = 4; //偏掉OAD
        
        //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
        tmpMsgLen = invoke_get_req(&pApdu[OOP_DATA_LIST_OFFSET + offset], dataLen, outTmp, msgmaxlenTmp);
        outTmp += tmpMsgLen;
        msgmaxlenTmp -= tmpMsgLen;

        offset += dataLen; //APDU的内偏移

        msglen += tmpMsgLen;
    }

    //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
    if(msglen != 0)
    {
        //GET-RESPONSE
        pOutMsg[0] = pApdu[0]|0x80;
        msglen++;

        //GetRespinseNormal 和 PIID-ACD
        uint16 i = OOP_DATA_LIST_OFFSET;
        while(--i)
        {
            pOutMsg[i] = pApdu[i];
            msglen++;
        }

        //TODO 跟随上报信息域
        pOutMsg[msglen ++] = 0;

        //时间标签域
        pOutMsg[msglen++] = 0;
    }

    ACUDP_FMT_TRACE("【end】 get_normal_list_req() 【结束】...msglen=%d \n", msglen);
    return msglen;
}


/*********************************************************************
 功能描述：读取一个对象属性
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 get_normal_req(uint8 *pApdu, uint16 nApduLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    /*GetRequestNormal∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         一个对象属性描述符 OAD，
    }*/

    ACUDP_FMT_TRACE("【start】 get_normal_req() 【开始】...\n");
    uint16 msglen = 0;

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    msglen = invoke_get_req(&pApdu[OOP_DATA_OFFSET], nApduLen-OOP_DATA_OFFSET, &pOutMsg[OOP_DATA_OFFSET], msgmaxlen-OOP_DATA_OFFSET);

    //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
    if(msglen != 0)
    {        
        //GET-RESPONSE
        pOutMsg[0] = pApdu[0]|0x80;
        msglen++;

        //GetRespinseNormal 和 PIID-ACD
        uint16 i = OOP_DATA_OFFSET;
        while(--i)
        {
            pOutMsg[i] = pApdu[i];
            msglen++;
        }

        //TODO 跟随上报信息域
        pOutMsg[msglen ++] = 0;

        //时间标签域
        pOutMsg[msglen++] = 0;
        
    }

    ACUDP_FMT_TRACE("【end】 get_normal_req() 【结束】...msglen=%d \n", msglen);
    return msglen;
}

/**
*********************************************************************
* @name：       set_err_segement_framing
* @brief：      SET服务的错误帧
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               uint16 msglen                消息长度
               headLen                      APDU头长度
* @param[out]：uint8 *pOutMsg                返回消息
* @return    ：返回消息长度
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
uint16 set_err_seg_framing(OOP_OAD_U oad, OOP_DAR_E dar, uint8 *pOutMsg, uint16 msgmaxlen)
{
    uint16 offset = 4;

    //OAD
    memcpy_r(pOutMsg, &oad, sizeof(uint32));

    //DAR
    pOutMsg[offset++] = dar;

    return offset;
}

/*********************************************************************
 功能描述：处理单个oad的解帧
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 set_single_oad_buff(OOP_OAD_U *serviceOAD, const uint8 *pInMsg, uint16 inLen, RESULT_DATA_T *outData, uint16 msgmaxlen, BOOL bHasOad, int datatype)
{
    OOP_DAR_E oopDar = DATA_SUCCESS;
    OAD_INFO_T pstOadInfo = {0};

    ACUDP_FMT_TRACE("set_single_oad_buff() serviceOAD->value=%08x, msgmaxlen=%d, bHasOad=%d, datatype=%d \n", serviceOAD->value, msgmaxlen, bHasOad, datatype);
    
    if (class_oadinfo_get(*serviceOAD, &pstOadInfo))
    {
        ACUDP_FMT_TRACE("set_single_oad_buff() pstOadInfo.pTab->oad.value=%08x, class=%d, OIA1=%d, OIA2=%d, dataType=%d, dataLen=%d, eData=%d \n", 
            pstOadInfo.pTab->oad.value, pstOadInfo.pTab->class, 
            pstOadInfo.pTab->OIA1, pstOadInfo.pTab->OIA2, pstOadInfo.pTab->dataType, pstOadInfo.pTab->dataLen, pstOadInfo.pTab->eData);

        switch(pstOadInfo.pTab->oad.value)
        {
            case 0x300F0600:   //设置交采电压（电流）逆相序事件参数
            case 0x300F0900:   //设置事件有效标记
            {
                return class7_invoke_set(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            case 0x40000200:   //设定时间
            {
                //return class8_invoke_set_time(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
                return DATA_SUCCESS;
            }
                break;
            case 0x40080200:			   //备用套时区表切换时间
            case 0x40090200:			   //备用套日时段切换时间
            {
                return class8_invoke_set_change_time(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            case 0x400C0200:   //时区时段数
            {
                return class8_invoke_set_time_region(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            case 0x40140200:   //当前套时区表
            {
                return class8_invoke_set_time_zone(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            case 0x40150200:   //备用套时区表
            {
                return class8_invoke_set_time_zone(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            case 0x40160200:   //当前套日时段表
            {
                return class8_invoke_set_day_period(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            case 0x40170200:   //备用套日时段表
            {
                return class8_invoke_set_day_period(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            case 0x40300200:   //设置交采电压合格率参数
            {
                return class8_invoke_set_volt_rate(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            case 0x41000200:   //需量周期
            {
                return class8_invoke_set_demand_period(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            case 0x41010200:   //滑差时间
            {
                return class8_invoke_set_slip_time(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;

            case 0x41120200:			   //有功组合方式特征字
            case 0x41130200:			   //无功组合方式1特征字
            case 0x41140200:			   //无功组合方式2特征字
            {
                return class8_invoke_set_feature(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            
            default:
                oopDar = DATA_OBJECT_UNDEFINED;
                break;
        }

    }
    else
    {
        oopDar = DATA_OBJECT_UNDEFINED;
        ACUDP_FMT_TRACE("get_single_oad_buff() class_oadinfo_get(*serviceOAD, &pstOadInfo) 失败! \n");
    }

    return oopDar;
}

/*********************************************************************
 功能描述：处理set服务的oad
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 invoke_set_req(const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    OOP_DAR_E oopDar = DATA_OTHER_REASON;
    uint16 msglen = 0;
    OOP_OAD_U serviceOAD = {0};
    RESULT_DATA_T outData;
    memset(&outData, 0x00, sizeof(RESULT_DATA_T));
    int dataType = -1;//普通数据1, 交采参数2
    
    uint16 index = 0;
    uint8 bufftmp[8192] = {0};

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));
    ACUDP_FMT_TRACE("invoke_set_req() serviceOAD=%08x \n", serviceOAD.value);

    //检查oad是否支持
    dataType = check_oad_is_valid(&serviceOAD, SERVERID_SETREQUEST);
    if(dataType < 0)  //不支持oad
    {
        ACUDP_FMT_TRACE("invoke_set_req() if(check_oad_is_valid(&serviceOAD) < 0) 不支持! \n", serviceOAD.value);
        memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
        index += sizeof(OOP_OAD_U);
        bufftmp[index] = DATA_OBJECT_UNDEFINED;  //(OAD+执行结果)错误码:对象未定义
        index ++;

        if(index > 0)
        {
            msglen = index;
            if(msglen > msgmaxlen)
            {
                msglen = msgmaxlen;
            }
            memcpy(pOutMsg, &bufftmp[0], msglen);
        }
        ACUDP_FMT_TRACE("invoke_set_req() 直接赋值错误码(对象未定义) index=%d, msglen=%d, msgmaxlen=%d \n", index, msglen, msgmaxlen);
        
        return msglen;
    }

    //单oad数据
    {
        ACUDP_FMT_TRACE("invoke_set_req() 单oad数据 \n");
        oopDar = set_single_oad_buff(&serviceOAD, pInMsg, inLen, &outData, msgmaxlen, TRUE, dataType);
    }

    memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
    index += sizeof(OOP_OAD_U);
    bufftmp[index] = oopDar;  //(OAD+执行结果)
    index ++;

    if(index > 0)
    {
        msglen = index;
        if(msglen > msgmaxlen)
        {
            msglen = msgmaxlen;
        }
        memcpy(pOutMsg, &bufftmp[0], msglen);
    }
    
    ACUDP_FMT_TRACE("invoke_set_req() 返回执行结果 index=%d, msglen=%d, msgmaxlen=%d, oopDar=%d \n", index, msglen, msgmaxlen, oopDar);
    return msglen;
}


/*********************************************************************
 功能描述：设置若干对象属性
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 set_normal_list_req(uint8 *pApdu, uint16 nApduLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    /*SetRequestNormalList∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         若干个对象属性            SEQUENCE OF
         {
            一个对象属性描述符 OAD，
            数据                 Data
         }
    }*/

    ACUDP_FMT_TRACE("【start】 set_normal_list_req() 【开始】...\n");
    
    ACUDP_FMT_TRACE("【end】 get_normal_list_req() 【结束】...\n");
    return 0;
}


/*********************************************************************
 功能描述：设置一个对象属性
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 set_normal_req(uint8 *pApdu, uint16 nApduLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    /*SetRequestNormal∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         一个对象属性描述符 OAD，
         数据                 Data
    }*/

    ACUDP_FMT_TRACE("【start】 set_normal_req() 【开始】...\n");
    uint16 msglen = 0;

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    msglen = invoke_set_req(&pApdu[OOP_DATA_OFFSET], nApduLen-OOP_DATA_OFFSET, &pOutMsg[OOP_DATA_OFFSET], msgmaxlen-OOP_DATA_OFFSET);

    //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
    if(msglen != 0)
    {        
        //SET-RESPONSE
        pOutMsg[0] = pApdu[0]|0x80;
        msglen++;

        //GetRespinseNormal 和 PIID-ACD
        uint16 i = OOP_DATA_OFFSET;
        while(--i)
        {
            pOutMsg[i] = pApdu[i];
            msglen++;
        }

        //TODO 跟随上报信息域
        pOutMsg[msglen ++] = 0;

        //时间标签域
        pOutMsg[msglen++] = 0;
        
    }

    ACUDP_FMT_TRACE("【end】 set_normal_req() 【结束】...msglen=%d \n", msglen);
    return msglen;
}

/*********************************************************************
 功能描述：处理单个oad的解帧
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 act_single_oad_buff(OOP_OAD_U *serviceOAD, const uint8 *pInMsg, uint16 inLen, RESULT_DATA_T *outData, uint16 msgmaxlen, BOOL bHasOad, int datatype)
{
    OOP_DAR_E oopDar = DATA_SUCCESS;
    OAD_INFO_T pstOadInfo = {0};

    ACUDP_FMT_TRACE("act_single_oad_buff() serviceOAD->value=%08x, msgmaxlen=%d, bHasOad=%d, datatype=%d \n", serviceOAD->value, msgmaxlen, bHasOad, datatype);
    
    if (class_omdinfo_get(*serviceOAD, &pstOadInfo))
    {
        ACUDP_FMT_TRACE("act_single_oad_buff() pstOadInfo.pTab->oad.value=%08x, class=%d, OIA1=%d, OIA2=%d, dataType=%d, dataLen=%d, eData=%d \n", 
            pstOadInfo.pTab->oad.value, pstOadInfo.pTab->class, 
            pstOadInfo.pTab->OIA1, pstOadInfo.pTab->OIA2, pstOadInfo.pTab->dataType, pstOadInfo.pTab->dataLen, pstOadInfo.pTab->eData);

        switch(serviceOAD->value)
        {
            case 0x50040500:   //删除交采冻结关联列
            case 0x50060500:   //删除交采冻结关联列
            {
                return frozen_cfg_unit_del(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;

            case 0x50040700:   //配置交采冻结关联列            
            case 0x50060700:   //配置交采冻结关联列
            {
                return frozen_cfg_batch_add(&pstOadInfo, pInMsg, inLen, outData, msgmaxlen);
            }
                break;
            
            default:
                oopDar = DATA_OBJECT_UNDEFINED;
                break;
        }

    }
    else
    {
        oopDar = DATA_OBJECT_UNDEFINED;
        ACUDP_FMT_TRACE("act_single_oad_buff() class_oadinfo_get(*serviceOAD, &pstOadInfo) 失败! \n");
    }

    return oopDar;
}


/*********************************************************************
 功能描述：处理act服务的omd
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 invoke_act_req(const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    OOP_DAR_E oopDar = DATA_OTHER_REASON;
    uint16 msglen = 0;
    OOP_OAD_U serviceOAD = {0};
    RESULT_DATA_T outData;
    memset(&outData, 0x00, sizeof(RESULT_DATA_T));
    int dataType = -1;//普通数据1, 交采参数2
    
    uint16 index = 0;
    uint8 bufftmp[8192] = {0};

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));
    ACUDP_FMT_TRACE("invoke_act_req() serviceOAD=%08x \n", serviceOAD.value);

    //检查oad是否支持
    dataType = check_oad_is_valid(&serviceOAD, SERVERID_ACTIONREQUEST);
    if(dataType < 0)  //不支持oad
    {
        ACUDP_FMT_TRACE("invoke_act_req() if(check_oad_is_valid(&serviceOAD) < 0) 不支持! \n", serviceOAD.value);
        memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
        index += sizeof(OOP_OAD_U);
        bufftmp[index] = DATA_READ_WRITE_DENIED;  //(OAD+执行结果)错误码:拒绝读写
        index ++;

        if(index > 0)
        {
            msglen = index;
            if(msglen > msgmaxlen)
            {
                msglen = msgmaxlen;
            }
            memcpy(pOutMsg, &bufftmp[0], msglen);
        }
        ACUDP_FMT_TRACE("invoke_act_req() 直接赋值错误码(对象未定义) index=%d, msglen=%d, msgmaxlen=%d \n", index, msglen, msgmaxlen);
        
        return msglen;
    }

    //单oad数据
    {
        ACUDP_FMT_TRACE("invoke_act_req() 单oad数据 \n");
        oopDar = act_single_oad_buff(&serviceOAD, pInMsg, inLen, &outData, msgmaxlen, TRUE, dataType);
    }

    memcpy_r(&bufftmp[index], &serviceOAD, 4);  //0AD
    index += sizeof(OOP_OAD_U);
    bufftmp[index] = oopDar;  //(OAD+执行结果)
    index ++;

    if(index > 0)
    {
        msglen = index;
        if(msglen > msgmaxlen)
        {
            msglen = msgmaxlen;
        }
        memcpy(pOutMsg, &bufftmp[0], msglen);
    }
    
    ACUDP_FMT_TRACE("invoke_act_req() 返回执行结果 index=%d, msglen=%d, msgmaxlen=%d, oopDar=%d \n", index, msglen, msgmaxlen, oopDar);
    return msglen;
}


/*********************************************************************
 功能描述：请求操作若干个对象方法
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 act_normal_list_req(uint8 *pApdu, uint16 nApduLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    /*SetRequestNormalList∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         若干个对象属性            SEQUENCE OF
         {
            一个对象属性描述符 OAD，
            数据                 Data
         }
    }*/

    ACUDP_FMT_TRACE("【start】 act_normal_list_req() 【开始】...\n");
    
    ACUDP_FMT_TRACE("【end】 act_normal_list_req() 【结束】...\n");
    return 0;
}


/*********************************************************************
 功能描述：请求操作一个对象方法
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 act_normal_req(uint8 *pApdu, uint16 nApduLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    /*SetRequestNormal∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         一个对象属性描述符 OAD，
         数据                 Data
    }*/

    ACUDP_FMT_TRACE("【start】 act_normal_req() 【开始】...\n");
    uint16 msglen = 0;

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    msglen = invoke_act_req(&pApdu[OOP_DATA_OFFSET], nApduLen-OOP_DATA_OFFSET, &pOutMsg[OOP_DATA_OFFSET], msgmaxlen-OOP_DATA_OFFSET);

    //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
    if(msglen != 0)
    {        
        //ACT-RESPONSE
        pOutMsg[0] = pApdu[0]|0x80;
        msglen++;

        //ActRespinseNormal 和 PIID-ACD
        uint16 i = OOP_DATA_OFFSET;
        while(--i)
        {
            pOutMsg[i] = pApdu[i];
            msglen++;
        }

        //TODO 跟随上报信息域
        pOutMsg[msglen ++] = 0;

        //时间标签域
        pOutMsg[msglen++] = 0;
        
    }

    ACUDP_FMT_TRACE("【end】 act_normal_req() 【结束】...msglen=%d \n", msglen);
    return msglen;
}

/*********************************************************************
 功能描述：处理get服务的oad(记录型)
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 invoke_get_record_req(const uint8 *pInMsg, uint16 inLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    OOP_DAR_E oopDar = DATA_OTHER_REASON;
    uint16 msglen = 0;
    OOP_OAD_U serviceOAD = {0};
    OAD_INFO_T pstOdaInfo = {0};
    RESULT_DATA_T outData;
    memset(&outData, 0x00, sizeof(RESULT_DATA_T));
    int dataType = -1;//普通数据1, 交采参数2
    
    uint16 index = 0;

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));
    ACUDP_FMT_TRACE("invoke_get_record_req() serviceOAD=%08x \n", serviceOAD.value);

    //检查oad是否支持
    dataType = check_oad_is_valid(&serviceOAD, SERVERID_GETREQUEST);
    if(dataType < 0)  //不支持oad
    {
        return msglen;
    }

    //读取记录型数据
    pstOdaInfo.isRec = TRUE;

    //查找oad信息
    if(class_oadinfo_get(serviceOAD, &pstOdaInfo))
    {
        switch(serviceOAD.value)
        {
            case 0x300F0200://电压逆向序事件
            {
                oopDar = class7_invoke_get(&pstOdaInfo, pInMsg, inLen, &outData, msgmaxlen);
                if (oopDar == DATA_SUCCESS)
                {
                    msglen = outData.resultDataLen;
                }
            }break;

            case 0x50040200://日冻结
            {
                oopDar = class9_invoke_get(&pstOdaInfo, pInMsg, inLen, &outData, msgmaxlen);
                if (oopDar == DATA_SUCCESS)
                {
                    msglen = outData.resultDataLen;
                }
            }break;

            case 0x50060200://月冻结
            {
                oopDar = class9_invoke_get(&pstOdaInfo, pInMsg, inLen, &outData, msgmaxlen);
                if (oopDar == DATA_SUCCESS)
                {
                    msglen = outData.resultDataLen;
                }
            }break;
            
            default:
                break;
        }
    }

    memcpy(pOutMsg, outData.pResultData, msglen);
    
    ACUDP_FMT_TRACE("invoke_get_req() index=%d, msglen=%d, msgmaxlen=%d \n", index, msglen, msgmaxlen);
    return msglen;
}


/*********************************************************************
 功能描述：读取一个记录型对象属性
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 get_record_req(uint8 *pApdu, uint16 nApduLen, uint8 *pOutMsg, uint16 msgmaxlen)
{
    /*GetRequestNormal∷=SEQUENCE
    {
         服务序号-优先级           PIID，
         一个对象属性描述符 OAD，
    }*/

    ACUDP_FMT_TRACE("【start】 get_record_req() 【开始】...\n");
    uint16 msglen = 0;

    //传入的报文是PIID之后的部分，传出的报文是PIID-ACD以后的部分
    msglen = invoke_get_record_req(&pApdu[OOP_DATA_OFFSET], nApduLen-OOP_DATA_OFFSET, &pOutMsg[OOP_DATA_OFFSET], msgmaxlen-OOP_DATA_OFFSET);

    //加上PIID-ACD之前的部分，和两个OPTIONAL 时戳与跟随上报信息
    if(msglen != 0)
    {        
        //GET-RESPONSE
        pOutMsg[0] = pApdu[0]|0x80;
        msglen++;

        //GetRespinseNormal 和 PIID-ACD
        uint16 i = OOP_DATA_OFFSET;
        while(--i)
        {
            pOutMsg[i] = pApdu[i];
            msglen++;
        }

        //TODO 跟随上报信息域
        pOutMsg[msglen ++] = 0;

        //时间标签域
        pOutMsg[msglen++] = 0;
        
    }

    ACUDP_FMT_TRACE("【end】 get_record_req() 【结束】...msglen=%d \n", msglen);
    return msglen;
}


/*********************************************************************
 功能描述：UDP服务 APDU解析
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
uint16 udp_apdu_analyse(uint8 *pApdu, uint16 nApduLen, uint8 *pOutApdu, uint16 msgmaxlen)
{
    uint16 nOutApduLen = 0;
    uint8  sertype = 0;  //客户机应用层数据单元类型
    uint8  reqtype = 0;  //读取请求类型

    //客户机应用层数据单元类型
    sertype = pApdu[0];
    ACUDP_FMT_TRACE("udp_apdu_analyse() 客户机应用层服务类型sertype=%d \n", sertype);
    
    if(sertype == SERVERID_GETREQUEST)// 读取请求
    {
        reqtype = pApdu[1];
        ACUDP_FMT_TRACE("udp_apdu_analyse()读取请求: 客户机应用层数据单元类型reqtype=%d \n", reqtype);
        switch(reqtype)
        {
            case REQ_GET_NORMAL:             //读取一个对象属性
                nOutApduLen = get_normal_req(pApdu, nApduLen, pOutApdu, msgmaxlen);
                break;
            
            case REQ_GET_NORMAL_LIST:        //读取若干对象属性
                nOutApduLen = get_normal_list_req(pApdu, nApduLen, pOutApdu, msgmaxlen);
                break; 

            case REQ_GET_RECORD:             //读取一个记录型对象属性
                nOutApduLen = get_record_req(pApdu, nApduLen, pOutApdu, msgmaxlen);
                break;
            
            default:
                return 0;
        }
    }
    else if(sertype == SERVERID_SETREQUEST)// 设置请求
    {
        reqtype = pApdu[1];
        ACUDP_FMT_TRACE("udp_apdu_analyse()设置请求: 客户机应用层数据单元类型reqtype=%d \n", reqtype);
        switch(reqtype)
        {
            case REQ_SET_NORMAL:             //设置一个对象属性
                nOutApduLen = set_normal_req(pApdu, nApduLen, pOutApdu, msgmaxlen);
                break;
            
            case REQ_SET_NORMAL_LIST:        //设置若干对象属性
                nOutApduLen = set_normal_list_req(pApdu, nApduLen, pOutApdu, msgmaxlen);
                break; 
            
            default:
                return 0;
        }
    }
    else if(sertype == SERVERID_ACTIONREQUEST)// 方法请求
    {
        reqtype = pApdu[1];
        ACUDP_FMT_TRACE("udp_apdu_analyse()方法请求: 客户机应用层数据单元类型reqtype=%d \n", reqtype);
        switch(reqtype)
        {
            case REQ_ACT_NORMAL:             //请求操作一个对象方法
                nOutApduLen = act_normal_req(pApdu, nApduLen, pOutApdu, msgmaxlen);
                break;
            
            case REQ_ACT_NORMAL_LIST:        //请求操作若干个对象方法
                nOutApduLen = act_normal_list_req(pApdu, nApduLen, pOutApdu, msgmaxlen);
                break; 
            
            default:
                return 0;
        }
    }
    else
    {
        ACUDP_FMT_TRACE("udp_apdu_analyse() 不支持的应用层服务!\n");
        return 0;
    }

    return nOutApduLen;
}


