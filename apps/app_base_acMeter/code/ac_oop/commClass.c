/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：规约解析公共函数
* @date： 2019.12.03
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"

const IC_COV_T g_ic_cov_op[OOP_IC_MAX] = 
{
     {1 , data_to_buf_class1 ,  NULL},
     {2 , data_to_buf_class2 ,  NULL},
     {3 , data_to_buf_class3 ,  NULL},
     {4 , data_to_buf_class4 ,  NULL},
     {5 , data_to_buf_class5 ,  NULL},
     {6 , data_to_buf_class6 ,  NULL},
     {8 , data_to_buf_class8 ,  NULL},
};

//翻转复制
void *memcpy_r(void *dest, const void *src, int count)
{
    void *ptr = dest;
    
    if(NULL == dest || NULL == src || count <= 0)  
    {   
        return NULL;    
    }
    
    while(count--)
    {
        *(char *)dest++ = *(char *)(src+count); 
    }

    return ptr;
}


/**********************************************************************
* @name      : set_len_offset
* @brief     ：设置元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：const uint8 *indata      数据Buffer
* @return    ：长度
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
    else if(eleLen <= 0xffff)
    {
        indata[0] = 0x82;
        indata[1] = (uint8)((eleLen >> 8) & 0xff);
        indata[2] = (uint8)(eleLen & 0xff);
        return 3;
    }
        
    return 0;
}


/**
*********************************************************************
* @name：       basic_type_check
* @brief：      检查数据类型是否符合能处理的基本型
* @param[in]： dataType   数据类型
* @return    ：检查是否通过 1-通过，0-不通过
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN basic_type_check(DATA_E dstType)
{
    switch(dstType)
    {
        case E_NULL                : return TRUE;
        case E_ARRAY               : return TRUE;    
        case E_STRUCTURE           : return TRUE;
        case E_BOOL                : return TRUE;  
        case E_BIT_STRING          : return TRUE;
        case E_OOP_BITSTR8_T       : return TRUE;
        case E_OOP_BITSTR64_T      : return TRUE;
        case E_OOP_BITSTR128_T     : return TRUE;
        case E_DOUBLE_LONG         : return TRUE;    
        case E_DOUBLE_LONG_UNSIGNED: return TRUE;
        case E_OCTET_STRING        : return TRUE;  
        case E_VISIBLE_STRING      : return TRUE;
        case E_UTF8_STRING         : return TRUE;    
        case E_INTEGER             : return TRUE;
        case E_LONG                : return TRUE;  
        case E_UNSIGNED            : return TRUE;
        case E_LONG_UNSIGNED       : return TRUE;    
        case E_LONG64              : return TRUE;
        case E_LONG64_UNSIGNED     : return TRUE;  
        case E_ENUM                : return TRUE;
        case E_FLOAT32             : return TRUE;    
        case E_FLOAT64             : return TRUE;
        case E_DATETIME            : return TRUE;  
        case E_DATE                : return TRUE;
        case E_TIME                : return TRUE;    
        case E_DATETIME_S          : return TRUE;
        case E_OI                  : return TRUE;  
        case E_OAD                 : return TRUE;
        case E_ROAD                : return TRUE;    
        case E_OMD                 : return TRUE;
        case E_TI                  : return TRUE;  
        case E_TSA                 : return TRUE;
        case E_MAC                 : return TRUE;    
        case E_RN                  : return TRUE;
        case E_REGION              : return TRUE;  
        case E_SCALER_UNIT         : return TRUE;
        case E_RSD                 : return TRUE;    
        case E_CSD                 : return TRUE;
        case E_MS                  : return TRUE;  
        case E_SID                 : return TRUE;
        case E_SIDMAC              : return TRUE;    
        case E_COMDCB              : return TRUE;
        case E_RCSD                : return TRUE;  
        case E_OOP_VISIBLEVAR_T    : return TRUE; 
        case E_OOP_VISIBLESTR4_T   : return TRUE; 
        case E_OOP_VISIBLESTR6_T   : return TRUE; 
        case E_OOP_VISIBLESTR8_T   : return TRUE; 
        case E_OOP_VISIBLEVAR32_T  : return TRUE;
        case E_OOP_OCTETVAR16_T    : return TRUE;
        case E_OOP_OCTETVAR256_T   : return TRUE;
        case E_OOP_OCTETVAR1K_T    : return TRUE;
        case E_OOP_OCTETVAR2K_T    : return TRUE;
        case E_OCTET_STR2_T        : return TRUE;
        case E_OCTET_STR3_T        : return TRUE;
        case E_OCTET_STR4_T        : return TRUE;
        case E_OCTET_STR32_T       : return TRUE;
        case E_OOP_SA_T            : return TRUE;
        case E_OOP_BITSTR16_T      : return TRUE;

        default: return FALSE;        
    }

    return FALSE;
}

/**
*********************************************************************
* @name：       data_type_oop_to_edata
* @brief：      数据类型转换
* @param[in] ：eData       待转类型
* @parma[ouy]: oopData    目的类型
* @return    ：转是否成功 1-成功，0-不成功
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN data_type_oop_to_edata(OOP_DATATYPE_E oopData, DATA_E *eData)
{
    switch(oopData)
    {
        case DT_NULL                : {*eData = E_NULL                ; return TRUE;}
        case DT_ARRAY               : {*eData = E_ARRAY               ; return TRUE;}      
        case DT_STRUCTURE           : {*eData = E_STRUCTURE           ; return TRUE;}       
        case DT_BOOL                : {*eData = E_BOOL                ; return TRUE;}     
        case DT_BIT_STRING          : {*eData = E_BIT_STRING          ; return TRUE;}    
        case DT_DOUBLE_LONG         : {*eData = E_DOUBLE_LONG         ; return TRUE;}   
        case DT_DOUBLE_LONG_UNSIGNED: {*eData = E_DOUBLE_LONG_UNSIGNED; return TRUE;}
        case DT_OCTET_STRING        : {*eData = E_OCTET_STRING        ; return TRUE;}   
        case DT_VISIBLE_STRING      : {*eData = E_VISIBLE_STRING      ; return TRUE;}  
        case DT_UTF8_STRING         : {*eData = E_UTF8_STRING         ; return TRUE;}      
        case DT_INTEGER             : {*eData = E_INTEGER             ; return TRUE;}       
        case DT_LONG                : {*eData = E_LONG                ; return TRUE;}  
        case DT_UNSIGNED            : {*eData = E_UNSIGNED            ; return TRUE;}
        case DT_LONG_UNSIGNED       : {*eData = E_LONG_UNSIGNED       ; return TRUE;}
        case DT_LONG64              : {*eData = E_LONG64              ; return TRUE;}
        case DT_LONG64_UNSIGNED     : {*eData = E_LONG64_UNSIGNED     ; return TRUE;}
        case DT_ENUM                : {*eData = E_ENUM                ; return TRUE;}
        case DT_FLOAT32             : {*eData = E_FLOAT32             ; return TRUE;}
        case DT_FLOAT64             : {*eData = E_FLOAT64             ; return TRUE;}
        case DT_DATETIME            : {*eData = E_DATETIME            ; return TRUE;}
        case DT_DATE                : {*eData = E_DATE                ; return TRUE;}
        case DT_TIME                : {*eData = E_TIME                ; return TRUE;}
        case DT_DATETIME_S          : {*eData = E_DATETIME_S          ; return TRUE;}
        case DT_OI                  : {*eData = E_OI                  ; return TRUE;}
        case DT_OAD                 : {*eData = E_OAD                 ; return TRUE;}
        case DT_ROAD                : {*eData = E_ROAD                ; return TRUE;}
        case DT_OMD                 : {*eData = E_OMD                 ; return TRUE;}
        case DT_TI                  : {*eData = E_TI                  ; return TRUE;}
        case DT_TSA                 : {*eData = E_TSA                 ; return TRUE;}
        case DT_MAC                 : {*eData = E_MAC                 ; return TRUE;}
        case DT_RN                  : {*eData = E_RN                  ; return TRUE;}
        case DT_REGION              : {*eData = E_REGION              ; return TRUE;}
        case DT_SCALER_UNIT         : {*eData = E_SCALER_UNIT         ; return TRUE;}
        case DT_RSD                 : {*eData = E_RSD                 ; return TRUE;}
        case DT_CSD                 : {*eData = E_CSD                 ; return TRUE;}
        case DT_MS                  : {*eData = E_MS                  ; return TRUE;}
        case DT_SID                 : {*eData = E_SID                 ; return TRUE;}
        case DT_SIDMAC              : {*eData = E_SIDMAC              ; return TRUE;}
        case DT_COMDCB              : {*eData = E_COMDCB              ; return TRUE;}
        case DT_RCSD                : {*eData = E_RCSD                ; return TRUE;}

        default : return FALSE;
    }

    return FALSE;
}

/**
*********************************************************************
* @name：       class_array_tofmt698
* @brief：      将数组类型数据转换成698格式报文
* @param[in]： pOadInfo   OAD信息
* @param[in]： pArrayInfo   数组信息
* @param[out]：convert   转换信息

* @return    ：= 0成功, != 0, 失败
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class_array_tofmt698(OAD_INFO_T *pOadInfo, ARRAY_INFO_T *pArrayInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    int ret = ERR_OK;
    uint8* dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 dOffset = *convert->dOffset;
    uint8* srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint32 sOffset = *convert->sOffset;    
    DATA_CONVERT_T unitConvert;
    uint32 unitSoffset = 0;
    uint32 i = 0; 
    uint8 offlen = 0;       //数组数量长度
    uint8 lenBuf[3] = {0};  //数组数量编码

    MEMZERO(&unitConvert, sizeof(unitConvert));

    if(pArrayInfo->unitNum > pArrayInfo->maxNum)
    {
        ACUDP_FMT_TRACE("class_array_tofmt698() srcData cross the border. nNum(%d) > maxNum(%d)\n", pArrayInfo->unitNum, pArrayInfo->maxNum);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    if(pOadInfo->noParse)
    {
        if(srcLen-sOffset < pArrayInfo->arrLen)
        {
            ACUDP_FMT_TRACE("class_array_tofmt698() srcData cross the border. srcLen(%d)-sOffset(%d) < arrLen(%d)\n", srcLen, sOffset, pArrayInfo->arrLen);
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }
        
        if(pOadInfo->nIndex == 0)
        {
            ret = memcpy_s(dstBuf+dOffset, dstLen-dOffset, srcBuf+sOffset, pArrayInfo->arrLen);
            if(ret != ERR_OK)
            {
                ACUDP_FMT_TRACE("class_array_tofmt698() memcpy_s failed. ret(%d)\n", ret);
                return DATA_OTHER_REASON;
            }
            
            dOffset += pArrayInfo->arrLen;
            sOffset += pArrayInfo->arrLen;
        }
        else
        {
            ret = memcpy_s(dstBuf+dOffset, dstLen-dOffset, (uint8*)pArrayInfo->pArray+(pOadInfo->nIndex-1)*pArrayInfo->unitSize, pArrayInfo->unitSize);
            if(ret != ERR_OK)
            {
                ACUDP_FMT_TRACE("class_array_tofmt698() memcpy_s failed. ret(%d)\n", ret);
                return DATA_OTHER_REASON;
            }
            
            dOffset += pArrayInfo->unitSize; 
            sOffset += pArrayInfo->unitSize;            
        }
    }
    else
    {
        unitConvert.dstBuf = dstBuf;
        unitConvert.dOffset = &dOffset;
        unitConvert.dstLen = dstLen;
        
        if(pOadInfo->nIndex == 0)
        {
            if(srcLen-sOffset < pArrayInfo->arrLen)
            {
                ACUDP_FMT_TRACE("class_array_tofmt698() srcData cross the border. srcLen(%d)-sOffset(%d) < arrLen(%d)\n", srcLen, sOffset, pArrayInfo->arrLen);
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            unitConvert.srcBuf = pArrayInfo->pArray;
            unitConvert.srcLen = pArrayInfo->unitSize*pArrayInfo->unitNum;
            
            //数组类型
            dstBuf[dOffset++] = DT_ARRAY;
            
            //数组成员个数
            offlen = set_len_offset(pArrayInfo->unitNum, lenBuf);
            
            ret = memcpy_s(&dstBuf[dOffset], dstLen-dOffset, lenBuf, offlen);
            if(ret != ERR_OK)
            {
                ACUDP_FMT_TRACE("class_array_tofmt698() memcpy_s failed. ret(%d)\n", ret);
                return DATA_OTHER_REASON;
            }
            
            dOffset += offlen;
			//ACUDP_FMT_TRACE("class_array_tofmt698() 数组类型+长度 dOffset=%d, *convert->sOffset=%d, *convert->dOffset=%d,  pArrayInfo->unitNum=%d \n", 
				//dOffset, *convert->sOffset, *convert->dOffset,  pArrayInfo->unitNum);

            for(i = 0; i < pArrayInfo->unitNum; i++)
            {
                unitSoffset = i*pArrayInfo->unitSize;
                unitConvert.sOffset = &unitSoffset;
                
                dar = pArrayInfo->func(pOadInfo, &unitConvert);

				//ACUDP_FMT_TRACE("class_array_tofmt698() i=%d, *convert->sOffset=%d, *convert->dOffset=%d, unitConvert.sOffset=%d, unitConvert.dOffset=%d, dOffset=%d \n", 
					//i, *convert->sOffset, *convert->dOffset, *unitConvert.sOffset, *unitConvert.dOffset, dOffset);
				
                if(dar != DATA_SUCCESS)
                {
                    ACUDP_FMT_TRACE("class_array_tofmt698() i=%d, block convert failed. oad(0x%08x), sOffset(%d), dOffset(%d)\n", 
                        i, pOadInfo->pTab->oad.value, unitConvert.sOffset, unitConvert.dOffset);
                    
                    return dar;                
                }
            }

            sOffset += pArrayInfo->arrLen;
        }
        else //其中一个单元
        {
            if(pOadInfo->subParse) //传入的是子结构
            {
                if(srcLen-sOffset < pArrayInfo->unitSize)
                {
                    ACUDP_FMT_TRACE("class_array_tofmt698() srcData cross the border. srcLen(%d)-sOffset(%d) < unitSize(%d)\n", srcLen, sOffset, pArrayInfo->unitSize);
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }

                unitConvert.srcBuf = srcBuf;
                unitConvert.srcLen = srcLen;  
                unitSoffset = sOffset;
            }
            else
            {
                if(pOadInfo->nIndex-1 > pArrayInfo->maxNum)
                {
                    ACUDP_FMT_TRACE("class_array_tofmt698() srcData cross the border. nIndex(%d) > maxNum(%d)\n", pOadInfo->nIndex-1, pArrayInfo->maxNum);
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }
                
                unitConvert.srcBuf = pArrayInfo->pArray;
                unitConvert.srcLen = pArrayInfo->unitSize*pArrayInfo->unitNum;                
                unitSoffset = (pOadInfo->nIndex-1)*pArrayInfo->unitSize;
            }
            
            unitConvert.sOffset = &unitSoffset;
            
            dar = pArrayInfo->func(pOadInfo, &unitConvert);
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_TRACE("class_array_tofmt698() unit convert failed. oad(0x%08x), sOffset(%d), dOffset(%d)\n", 
                    pOadInfo->pTab->oad.value, unitConvert.sOffset, unitConvert.dOffset);
                
                return dar;                
            }

            sOffset += unitSoffset;
        }
    }   

    if((dOffset > dstLen) ||(sOffset > srcLen))
    {
        ACUDP_FMT_TRACE("class_array_tofmt698() cross the border. dOffset(%d) > dstLen(%d) or sOffset(%d) > srcLen(%d)\n", dOffset, dstLen, sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset; 

	//ACUDP_FMT_TRACE("class_array_tofmt698() end: *convert->sOffset=%d, *convert->dOffset=%d, dOffset=%d \n", 
		//*convert->sOffset, *convert->dOffset, dOffset);
    
    return dar;
}

#if DESC("APDU D2B 非DATA内部的RSD&RCSD",1)

/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 apdu_tsa_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TSA_T data;
    uint8 checkLen = 0;

    MEMZERO(&data, sizeof(data));

    //TSA长度
    checkLen = srcBufTmp[offset++];

    data.af = srcBufTmp[offset++];

    if(data.len+2 != checkLen)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "check tsa size failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }
    
    memcpy(data.add, &srcBufTmp[offset], data.len+1); 
    offset += data.len+1;

    memcpy(dstData, &data, sizeof(data));
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_tsa_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TSA_T data;
    uint8 checkLen = 0;

    MEMZERO(&data, sizeof(data));
    
    //TSA类型
    if(srcBufTmp[offset++] != DT_TSA)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "check tsa type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    //TSA长度
    checkLen = srcBufTmp[offset++];

    data.af = srcBufTmp[offset++];

    if(data.len+2 != checkLen)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "check tsa size failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    if (data.flag == 0x01)
    {
        data.vxd = srcBufTmp[offset ++];
        memcpy(data.add, &srcBufTmp[offset], data.len); 
        offset += data.len;
    }
    else
    {
        memcpy(data.add, &srcBufTmp[offset], data.len+1); 
        offset += data.len + 1;
    }

    memcpy(dstData, &data, sizeof(data));
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：        basic_ms_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ms_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_MSP_T ms;
    uint32 i = 0;    

    /*MS∷=CHOICE
     { 
         无表计              [0]，
         全部用户地址           [1]，
         一组用户类型           [2] SEQUENCE OF unsigned，
         一组用户地址           [3] SEQUENCE OF TSA，
         一组配置序号           [4] SEQUENCE OF long-unsigned，
         一组用户类型区间 [5] SEQUENCE OF Region，
         一组用户地址区间 [6] SEQUENCE OF Region，
         一组配置序号区间 [7] SEQUENCE OF Region
     }*/
     
    MEMZERO(&ms, sizeof(ms));

    ms.choice = srcBufTmp[offset++];

    switch(ms.choice)
    {
        case 0: break;//无表计 
        case 1: break;//所有电能表
        
        case 2:       //一组用户类型
        {
            ms.size = srcBufTmp[offset++];
            ms.mst.size = ms.size;
            for(i = 0; i < ms.msi.size; i++)
            {
                memcpy(&ms.mst.type[i], &srcBufTmp[offset], sizeof(uint8));    
                offset += sizeof(uint8);
            }
        }break;

        case 3:       //一组用户地址
        {
            ms.size = srcBufTmp[offset++];
            ms.msa.size = ms.size;
            for(i = 0; i < ms.msa.size; i++)
            {
                dar = apdu_tsa_fromfmt698(srcBuf, srcLen, &offset, &ms.msa.add[i], sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_tsa_fromfmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }                     
            }
        }break;   

        case 4:       //一组配置序号
        {
            ms.size = srcBufTmp[offset++];
            ms.msi.size = ms.size;
            for(i = 0; i < ms.msi.size; i++)
            {
               memcpy_r(&ms.msi.idx[i], &srcBufTmp[offset], sizeof(uint16));
               offset += sizeof(uint16);               
            }

        }break;    

        case 5:       //一组用户类型区间
        {
            ms.size = srcBufTmp[offset++];
            ms.msrt.size = ms.size;
            for(i = 0; i < ms.msrt.size; i++)
            {
                ms.msrt.region[i].nType = srcBufTmp[offset++]; //区间类型

                dar = basic_buf_to_data(E_UNSIGNED,srcBuf, srcLen, &offset, &ms.msrt.region[i].start, sizeof(uint8));
                if(dar != DATA_SUCCESS)
                {
                    ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }  

                dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &offset, &ms.msrt.region[i].end, sizeof(uint8));
                if(dar != DATA_SUCCESS)
                {
                    ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }                
            }
        }break; 

        case 6:       //一组用户地址区间
        {
            ms.size = srcBufTmp[offset++];
            ms.msra.size = ms.size;
            for(i = 0; i < ms.msra.size; i++)
            {
                ms.msra.region[i].nType = srcBufTmp[offset++]; //区间类型
                
                dar = basic_tsa_fromfmt698(srcBuf, srcLen, &offset, &ms.msra.region[i].start, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_tsa_fromfmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }  
                
                dar = basic_tsa_fromfmt698(srcBuf, srcLen, &offset, &ms.msra.region[i].end, sizeof(OOP_TSA_T));
                if(dar != DATA_SUCCESS)
                {
                    ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_tsa_fromfmt698 failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }
            }
        }break;   

        case 7:       //一组配置序号区间
        {
            ms.size = srcBufTmp[offset++];
            ms.msri.size = ms.size;
            for(i = 0; i < ms.msra.size; i++)
            {
                ms.msri.region[i].nType = srcBufTmp[offset++]; //区间类型
                
                dar = basic_buf_to_data(E_LONG_UNSIGNED,srcBuf, srcLen, &offset, &ms.msri.region[i].start, sizeof(uint16));
                if(dar != DATA_SUCCESS)
                {
                    ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }  
                
                dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &offset, &ms.msri.region[i].end, sizeof(uint16));
                if(dar != DATA_SUCCESS)
                {
                    ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
                    return DATA_TYPE_UNMATCHED;           
                }
            }
        }break;          
#if AREA_ZHEJIANG
        case 8:       //一组配置序号
        {
            ms.size = srcBufTmp[offset++];
            ms.msdt.size = ms.size;
            for(i = 0; i < ms.msdt.size; i++)
            {
               memcpy_r(&ms.msdt.type[i], &srcBufTmp[offset], sizeof(uint16));
               offset += sizeof(uint16);               
            }

        }break;    
#endif
        
    }

    memcpy(dstData, &ms, sizeof(ms));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        apdu_ti_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ti_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TI_T data; 
    
    MEMZERO(&data, sizeof(data));

    data.unit = srcBufTmp[offset++];  
    
    memcpy_r(&data.value, &srcBufTmp[offset], sizeof(uint16));
    offset += sizeof(uint16);

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_datetime_s_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_DATETIME_S_T data; 
    
    MEMZERO(&data, sizeof(data));
    
    memcpy_r(&data.year, &srcBufTmp[offset], sizeof(uint16));
    offset += sizeof(uint16);

    data.month  = srcBufTmp[offset++];  
    data.day    = srcBufTmp[offset++];           
    data.hour   = srcBufTmp[offset++]; 
    data.minute = srcBufTmp[offset++]; 
    data.second = srcBufTmp[offset++]; 

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/

OOP_DAR_E apdu_variant_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_VARIANT_T variant;  
    DATA_E eData = 0;
    BOOLEAN bValue = 0;
    uint32 typelen = (uint32)&variant.tmVal-(uint32)&variant.type;
    
    MEMZERO(&variant, sizeof(variant));

    variant.type = srcBufTmp[offset++];
    ACUDP_FMT_TRACE("type(%d)\n", variant.type);    

    bValue = data_type_oop_to_edata(variant.type, &eData);
    if(bValue)
    {
        offset--; //由于下面函数还要解析一遍类型,union中任何一个变量地址可以代表union的地址
        dar = basic_buf_to_data(eData, srcBuf, srcLen, &offset, &variant.tmVal, sizeof(variant)-typelen);
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_buf_to_data failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;           
        } 
    }
    else
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "undefined variant type. offset(%d)", offset);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }
    
    switch(variant.type)
    {
        case DT_DATETIME_S: 
        {
            ACUDP_FMT_TRACE("%04d-%02d-%02d %02d:%02d:%02d\n", 
            variant.tmVal.year, variant.tmVal.month, variant.tmVal.day, 
            variant.tmVal.hour, variant.tmVal.minute, variant.tmVal.second);            
        }break;
        case DT_TI: 
        {
            ACUDP_FMT_TRACE("ti.unit(%d), ti.value(%d)\n", variant.ti.unit, variant.ti.value);            
        }break;        
    }
    
    memcpy(dstData, &variant, sizeof(variant));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel1_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTOR1_T selector;

    /*Selector1∷=SEQUENCE
    {
        对象属性描述符 OAD，
        数值             Data
    }*/
     
    MEMZERO(&selector, sizeof(selector));

    //OAD
    memcpy_r(&selector.oad, &srcBufTmp[offset], sizeof(uint32));
    offset += sizeof(uint32);
    ACUDP_FMT_TRACE("oda(0x%08x)\n", selector.oad.value);

    //Data
    dar = apdu_variant_fromfmt698(srcBuf, srcLen, &offset, &selector.value, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_variant_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel2_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTOR2_T selector;

    /*Selector2∷=SEQUENCE
    {
        对象属性描述符 OAD，
        起始值            Data，
        结束值            Data，
        数据间隔           Data
    }*/

    MEMZERO(&selector, sizeof(selector));

    //OAD
    memcpy_r(&selector.oad, &srcBufTmp[offset], sizeof(uint32));
    offset += sizeof(uint32);
    ACUDP_FMT_TRACE("oda(0x%08x)\n", selector.oad.value);

    //Data
    dar = apdu_variant_fromfmt698(srcBuf, srcLen, &offset, &selector.from, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("apdu_variant_fromfmt698 failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    //Data
    dar = apdu_variant_fromfmt698(srcBuf, srcLen, &offset, &selector.to, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("apdu_variant_fromfmt698 failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    //Data
    dar = apdu_variant_fromfmt698(srcBuf, srcLen, &offset, &selector.span, sizeof(OOP_VARIANT_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("apdu_variant_fromfmt698 failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel3_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTOR3_T selector;
    uint32 i = 0;

    /*Selector3∷=SEQUENCE OF Selector2*/

    MEMZERO(&selector, sizeof(selector));

    selector.nNum = srcBufTmp[offset++];
    ACUDP_FMT_TRACE("nNum(0x%08x)\n", selector.nNum);

    if (selector.nNum > MAX_SELECTOR2_NUM)
    {
        ACUDP_FMT_DEBUG("basic_ccsd_fromfmt698 failed. selector.nNum(%d) > MAX_SELECTOR2_NUM(%d)\n", selector.nNum, MAX_SELECTOR2_NUM);
        return DATA_TYPE_UNMATCHED;
    }

    for(i = 0; i< selector.nNum; i++)
    {
        dar = apdu_rsd_sel2_fromfmt698(srcBuf, srcLen, &offset, &selector.item[i], sizeof(OOP_SELECTOR2_T));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_DEBUG("basic_ccsd_fromfmt698 failed. dar(%d), offset(%d)\n", dar, offset);
            return DATA_TYPE_UNMATCHED;           
        }        
    }

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel4_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP4_T selector;

    /*Selector4∷=SEQUENCE
     {
        采集启动时间 date_time_s，
        表计集合         MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.time, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("basic_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel5_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP5_T selector;

    /*Selector5∷=SEQUENCE
     {
        采集存储时间 date_time_s，
        表计集合         MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.time, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel6_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP6_T selector;

    /*Selector6∷=SEQUENCE
     {
        采集启动时间起始值 date_time_s，
        采集启动时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_fromfmt698(srcBuf, srcLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ti_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel7_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP7_T selector;

    /*Selector7∷=SEQUENCE
     {
        采集存储时间起始值 date_time_s，
        采集存储时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_fromfmt698(srcBuf, srcLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ti_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel8_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP8_T selector;

    /*Selector8∷=SEQUENCE
     {
        采集成功时间起始值 date_time_s，
        采集成功时间结束值 date_time_s， 
        时间间隔               TI，
        表计集合               MS
     }*/

    MEMZERO(&selector, sizeof(selector));

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.start, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &selector.end, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ti_fromfmt698(srcBuf, srcLen, &offset, &selector.ti, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ti_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel9_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTOR9_T selector;

    /*Selector9∷=SEQUENCE
     {
        上第 n 次记录 unsigned  
     }*/
     
    MEMZERO(&selector, sizeof(selector));

    selector.nLast = srcBufTmp[offset++];
    ACUDP_FMT_TRACE("nLast(%d)\n", selector.nLast);

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}

/**
*********************************************************************
* @name：        basic_rsd_sel10_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_sel10_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_SELECTORP10_T selector;

    /*Selector10∷=SEQUENCE
     {
        上第 n 次记录 unsigned,
        表计集合               MS   
     }*/
     
    MEMZERO(&selector, sizeof(selector));

    selector.nTimes = srcBufTmp[offset++];

    dar = apdu_ms_fromfmt698(srcBuf, srcLen, &offset, &selector.ms, sizeof(OOP_MSP_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "basic_datetime_s_fromfmt698 failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &selector, sizeof(selector));

    *pOffset = offset;

    return dar; 
}


/**
*********************************************************************
* @name：        nontype_rsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_PRSD_T rsd; //

    /*RSD∷=CHOICE
    {
        不选择 [0] NULL，
        选择方法 1 [1] Selector1，*
        ......
    }*/

    MEMZERO(&rsd, sizeof(rsd));
    
    rsd.choice = srcBufTmp[offset++];
    ACUDP_FMT_TRACE("rsd.choice Selector%d.\n", rsd.choice);

    switch(rsd.choice)
    {
        case 0: //NULL
        {
            ACUDP_BUF_TRACE(srcBufTmp, srcLen, "NULL Selector. offset(%d)", offset);
        }break;

        case 1: //Selector1
        {
            dar = apdu_rsd_sel1_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel1, sizeof(OOP_SELECTOR1_T));        
        }break;
        
        case 2: //Selector2
        {
            dar = apdu_rsd_sel2_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel2, sizeof(OOP_SELECTOR2_T));        
        }break;
        
        case 3: //Selector3
        {
            dar = apdu_rsd_sel3_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel3, sizeof(OOP_SELECTOR3_T));        
        }break;
        case 4: //Selector4
        {
            dar = apdu_rsd_sel4_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel4, sizeof(OOP_SELECTORP4_T));     
        }break;
        
        case 5: //Selector5
        {
            dar = apdu_rsd_sel5_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel5, sizeof(OOP_SELECTORP5_T));     
        }break;
        
        case 6: //Selector6
        {
            dar = apdu_rsd_sel6_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel6, sizeof(OOP_SELECTORP6_T));
        }break;
        
        case 7: //Selector7
        {
            dar = apdu_rsd_sel7_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel7, sizeof(OOP_SELECTORP7_T));
        }break;
        
        case 8: //Selector8
        {
            dar = apdu_rsd_sel8_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel8, sizeof(OOP_SELECTORP8_T));
        }break;
      
        case 9: //Selector9
        {
            dar = apdu_rsd_sel9_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel9, sizeof(OOP_SELECTOR9_T));
        }break;
            
        case 10: //Selector10
        {
            dar = apdu_rsd_sel10_fromfmt698(srcBuf, srcLen, &offset, &rsd.sel10, sizeof(OOP_SELECTORP10_T));
        }break;

        default:
        {
            ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "undefined Selector. offset(%d)", offset);
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }break;            
    }

    memcpy(dstData, &rsd, sizeof(rsd));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_ccsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ccsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_CCSD_T csd; //APDU使用的CCSD，参数使用的CSD
    uint32 i = 0;
     
    MEMZERO(&csd, sizeof(csd));

    /*CSD∷=CHOICE
    {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
    }
    ROAD∷=SEQUENCE
    {
        对象属性描述符            OAD，
        关联对象属性描述符 SEQUENCE OF OAD
    }*/
    
    csd.choice = srcBufTmp[offset++];

    if(csd.choice == 0) //单OAD类型
    {
        memcpy_r(&csd.oad, &srcBufTmp[offset], sizeof(uint32)); //OAD
        offset += sizeof(uint32);
    }
    else if(csd.choice == 1)//记录型ROAD
    {
        memcpy_r(&csd.road.oadMain, &srcBufTmp[offset], sizeof(uint32)); //OAD
        offset += sizeof(uint32);   

        csd.road.oadCols.nNum = srcBufTmp[offset++]; //SEQUENCE个数

        for(i = 0; i < csd.road.oadCols.nNum; i++)
        {
            memcpy_r(&csd.road.oadCols.oad[i], &srcBufTmp[offset], sizeof(uint32)); //OAD
            offset += sizeof(uint32);                       
        }
    }
    else
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "ccsd choice failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }

    memcpy(dstData, &csd, sizeof(csd));

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_rcsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rcsd_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_RCSD_T rcsd;
    uint32 i = 0;
     
    MEMZERO(&rcsd, sizeof(rcsd));

    /*RCSD∷=SEQUENCE OF CSD*/
    
    rcsd.nNum = srcBufTmp[offset++];
    ACUDP_FMT_TRACE("rcsd.nNum(%d)\n", rcsd.nNum);

    for(i = 0; i < rcsd.nNum; i++)
    {
        dar = apdu_ccsd_fromfmt698(srcBuf, srcLen, &offset, &rcsd.cols[i], sizeof(OOP_CCSD_T));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ccsd_fromfmt698 failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;           
        }
        
        ACUDP_FMT_TRACE("[%d] choice(%d), oad(0x%08x)\n", i, rcsd.cols[i].choice, rcsd.cols[i].oad.value);        
    }
    
    memcpy(dstData, &rcsd, sizeof(rcsd));

    *pOffset = offset;

    return dar;
}


#endif

#if DESC("APDU B2D 非DATA内部的RSD&RCSD",1)
/**
*********************************************************************
* @name：        apdu_csd_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_ccsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_CCSD_T csd; //APDU使用的CCSD，参数使用的CSD   
    uint32 i = 0;
     
    memcpy(&csd, &srcDataTmp[0], srcLen);

    /*CSD∷=CHOICE
    {
        对象属性描述符              [0] OAD，
        记录型对象属性描述符 [1] ROAD
    }
    ROAD∷=SEQUENCE
    {
        对象属性描述符            OAD，
        关联对象属性描述符 SEQUENCE OF OAD
    }*/
    
    dstBufTmp[offset++] = csd.choice; //选择项

    if(csd.choice == 0) //单OAD类型
    {
        memcpy_r(&dstBufTmp[offset], &csd.oad, sizeof(uint32)); //OAD
        offset += sizeof(uint32);
    }
    else if(csd.choice == 1)//记录型ROAD
    {
        memcpy_r(&dstBufTmp[offset], &csd.road.oadMain, sizeof(uint32)); //OAD
        offset += sizeof(uint32);   

        dstBufTmp[offset++] = csd.road.oadCols.nNum; //SEQUENCE个数

        for(i = 0; i < csd.road.oadCols.nNum; i++)
        {
            memcpy_r(&dstBufTmp[offset], &csd.road.oadCols.oad[i], sizeof(uint32)); //OAD
            offset += sizeof(uint32);                       
        }
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @name：        basic_rcsd_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E apdu_rcsd_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_RCSD_T rcsd;
    uint32 i = 0;
     
    memcpy(&rcsd, &srcDataTmp[0], srcLen);

    /*RCSD∷=SEQUENCE OF CSD*/   
    
    dstBufTmp[offset++] = rcsd.nNum; //SEQUENCE个数
   
    //遍历RCSD中每个CSD        
    for(i = 0; i < rcsd.nNum; i++)
    {
        dar = apdu_ccsd_tofmt698(dstBuf, dstLen, &offset, &rcsd.cols[i], sizeof(OOP_CCSD_T));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_DEBUG(dstBuf, dstLen, "apdu_ccsd_tofmt698 failed. offset(%d)", offset);
            return DATA_TYPE_UNMATCHED;           
        }        
    }

    *pOffset = offset;

    return dar;
}

#endif

/**
*********************************************************************
* @name：        basic_visiblestr_data_to_buf(定长)
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_bitstr16_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
	int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_BITSTR16_T bitstring;

	//ACUDP_FMT_TRACE("basic_bitstr16_tofmt698() begin: *pOffset=%d, offset=%d \n", *pOffset, offset);

    memcpy(&bitstring, &srcDataTmp[0], sizeof(OOP_BITSTR16_T));

	//ACUDP_FMT_TRACE("basic_bitstr16_tofmt698() *pOffset=%d, offset=%d, &bitstring=%p, &offset=%p, srcLen=%d \n", 
		//*pOffset, offset, &bitstring, &offset, srcLen);

    //bitstring数据类型
    dstBufTmp[offset++] = DT_BIT_STRING;

    //bitstring的长度域
    dstBufTmp[offset++] = 16;

    //bitstring数据域
    memcpy_r(&dstBufTmp[offset], bitstring.nValue, 2);
    offset += 2;
    
    *pOffset=offset;

	//ACUDP_FMT_TRACE("basic_bitstr16_tofmt698() end: *pOffset=%d, offset=%d \n", *pOffset, offset);
    
    return nRet;
}


/**
*********************************************************************
* @name：        basic_datetime_s_tofmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_datetime_s_tofmt698(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;    
    uint8 *srcDataTmp = (uint8*)srcData;
    uint8 *dstBufTmp = (uint8*)dstBuf;
    OOP_DATETIME_S_T data;

    memcpy(&data, &srcDataTmp[0], srcLen);

    //数据类型
    dstBufTmp[offset++] = DT_DATETIME_S;
    
    memcpy_r(&dstBufTmp[offset], &data.year, sizeof(uint16));
    offset += sizeof(uint16);

    dstBufTmp[offset++] = data.month;  
    dstBufTmp[offset++] = data.day;           
    dstBufTmp[offset++] = data.hour; 
    dstBufTmp[offset++] = data.minute; 
    dstBufTmp[offset++] = data.second; 
    
    *pOffset = offset;

    return nRet;
}

/**
*********************************************************************
* @name：        basic_visiblestr_data_to_buf
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 basic_visiblestr32_tofmt698(void* dstBuf, uint32 dstLen, uint32* pOffset, void* srcData, uint32 srcLen)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;
    uint8* srcDataTmp = (uint8*)srcData;
    uint8* dstBufTmp = (uint8*)dstBuf;
    OOP_VISIBLESTR20_T visiblestr;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = { 0 }; //数组数量编码   
    uint32 i = 0;

    memcpy(&visiblestr, &srcDataTmp[0], srcLen);

    //visiblestr数据类型
    dstBufTmp[offset++] = DT_VISIBLE_STRING;

    //visiblestr的长度域
    offlen = set_len_offset(srcLen, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //visiblestr数据域
    for (i = 0; i < srcLen; i++)
    {
        dstBufTmp[offset++] = visiblestr.value[i];
    }
    *pOffset = offset;
    return nRet;
}

/**
*********************************************************************
* @name：        basic_data_to_buf
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_data_to_fmt698(DATA_E eData, DATA_CONVERT_T *convert)
{
	OOP_DAR_E dar = DATA_OTHER_REASON;
	uint32	sOffset = 0;
	uint32	dOffset = 0;
	uint8 *srcBuf = (uint8 *)convert->srcBuf;	 
	uint8 *dstBuf = (uint8 *)convert->dstBuf;
	uint32 srcLen = 0;
	uint32 dstLen = 0;	 
	uint32 dataLen = 0;

	 /*入参检查*/
	if ((convert == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL)|| 
		(convert->sOffset == NULL) || (convert->dOffset == NULL))
	{
		ACUDP_FMT_TRACE("basic_data_to_fmt698 check failed. ERR_PTR\n");
		return ERR_PTR;
	}

	if(!basic_type_check(eData))
	{
		ACUDP_FMT_TRACE("basic_type_check failed. eData(%u)\n", eData);		
	}
	
	sOffset = *convert->sOffset;
	dOffset = *convert->dOffset;
	srcLen = convert->srcLen;
	dstLen = convert->dstLen;

	//ACUDP_FMT_TRACE("basic_data_to_fmt698() begin: dOffset=%d, *convert->sOffset=%d, *convert->dOffset=%d, convert->srcLen=%d, convert->dstLen=%d \n", 
				//dOffset, *convert->sOffset, *convert->dOffset, convert->srcLen, convert->dstLen);

	switch (eData)
	{
		case E_NULL:							/**< NULL				*/
		{
			dstBuf[dOffset++] = DT_NULL;
			dar = ERR_OK;
			break;
		}

		case E_ARRAY:							/**< vector 			*/
		case E_STRUCTURE:						/**< struct 			*/
		case E_BOOL:							/**< BOOLEAN				*/
		case E_BIT_STRING:						/**< CBitString 		*/
        {
			break;
		}
        case E_OOP_BITSTR8_T:
        {
            dataLen = sizeof(uint8);
            
            if ((srcLen - sOffset) < dataLen)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            dstBuf[dOffset++] = DT_BIT_STRING;
            dstBuf[dOffset++] = 8;
            
            memcpy(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            dOffset += dataLen;
            sOffset += dataLen;

            dar = ERR_OK;
            break;
        }
		case E_OOP_BITSTR64_T:
		case E_OOP_BITSTR128_T:
		{
			break;
		}
		case E_OOP_BITSTR16_T:
        {
            dataLen = sizeof(OOP_BITSTR16_T);     
            dar = basic_bitstr16_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;

			//ACUDP_FMT_TRACE("basic_data_to_fmt698() case E_OOP_BITSTR16_T: dOffset=%d, *convert->sOffset=%d, *convert->dOffset=%d, convert->srcLen=%d, convert->dstLen=%d \n", 
				//dOffset, *convert->sOffset, *convert->dOffset, convert->srcLen, convert->dstLen);
            break;
        }
		case E_DOUBLE_LONG: 					/**< int				*/
		{
			dataLen = sizeof(int32);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_DOUBLE_LONG;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_DOUBLE_LONG_UNSIGNED:			/**< uint32 			*/
		{
			dataLen = sizeof(uint32);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_DOUBLE_LONG_UNSIGNED;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_OCTET_STR32_T:					 /**< octet-string(SIZE(32))   */
		case E_OCTET_STRING:					/**< COctetString		*/
		case E_OOP_OCTETVAR256_T:				 /**< COctetString		 */
		case E_OOP_OCTETVAR1K_T:				/**< COctetString		*/
		case E_OOP_OCTETVAR2K_T:				/**< COctetString		*/
		case E_OOP_OCTETVAR16_T:
		case E_OCTET_STR3_T:
		case E_OOP_SA_T:
		case E_OOP_VISIBLEVAR_T:
        {
            break;
        }

        case E_OOP_VISIBLESTR4_T:
        {
            dataLen = sizeof(OOP_VISIBLESTR4_T);
            srcLen = 0x04;
            dar = basic_visiblestr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }

        case E_OOP_VISIBLESTR6_T:
        {
            dataLen = sizeof(OOP_VISIBLESTR6_T);
            srcLen = 0x06;
            dar = basic_visiblestr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }

        case E_OOP_VISIBLESTR8_T:
        {
            dataLen = sizeof(OOP_VISIBLESTR8_T);
            srcLen = 0x08;
            dar = basic_visiblestr32_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;
            break;
        }

		case E_OOP_VISIBLEVAR32_T:
		case E_VISIBLE_STRING:					/**< string 			*/
		case E_UTF8_STRING: 					/**< string 			*/
		{
			break;
		}
		case E_INTEGER: 						/**< char				*/
		{
			dataLen = sizeof(char);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_INTEGER;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_LONG:							/**< int16				*/
		{
			dataLen = sizeof(int16);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_LONG;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_UNSIGNED:						/**< uint8				*/
		{
			dataLen = sizeof(uint8);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_UNSIGNED;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_LONG_UNSIGNED:					/**< uint16 			*/
		{
			dataLen = sizeof(uint16);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_LONG_UNSIGNED;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_LONG64:							/**< int64				*/
		{
			dataLen = sizeof(int64);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_LONG64;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_LONG64_UNSIGNED: 				/**< uint64 			*/
		{
			dataLen = sizeof(uint64);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_LONG64_UNSIGNED;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;

		}

		case E_ENUM:							/**< uint8				*/
		{
			dataLen = sizeof(uint8);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_ENUM;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_FLOAT32: 						/**< float32			*/
		{
			dataLen = sizeof(float32);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_FLOAT32;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_FLOAT64: 						/**< float64			*/
		{
			dataLen = sizeof(float64);
			
			if ((srcLen - sOffset) < dataLen)
			{
				dar = DATA_OTHER_REASON;
				break;
			}
			
			dstBuf[dOffset++] = DT_FLOAT64;
			
			memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
			dOffset += dataLen;
			sOffset += dataLen;

			dar = ERR_OK;
			break;
		}

		case E_DATETIME:						/**< DateTime			*/
			break;
		case E_DATE:							/**< OOP_DATE_T 		*/
			break;
		case E_TIME:							/**< OOP_TIME_T 		*/
			break;
		case E_DATETIME_S:						/**< DateTime_S 		*/
		{
            dataLen = sizeof(OOP_DATETIME_S_T);
            dar = basic_datetime_s_tofmt698(dstBuf, dstLen, &dOffset, srcBuf, srcLen);
            sOffset += dataLen;            
        }
			break;
		case E_OI:								/**< OI 				*/
		case E_OAD: 							/**< OOP_OAD_U			*/
		case E_ROAD:							/**< OOP_ROAD_T 		*/
		case E_OMD: 							/**< OOP_OMD_U			*/
		case E_TI:								/**< 时间间隔			*/
		case E_TSA: 							/**< 设备地址			*/
		case E_MAC: 							/**< OOP_MAC_T签名		*/
		case E_RN:								/**< 随机数				*/
		case E_REGION:							/**< 区间类型			*/
		case E_SCALER_UNIT: 					/**< 换算单位			*/
		case E_RSD: 							/**< 行选择符			*/
		case E_CSD: 							/**< 列选择符			*/
		case E_MS:								/**< 电能表集合			*/
		case E_SID: 							/**< 安全标识			*/
		case E_SIDMAC:							/**< 安全标识+OOP_MAC_T */
		case E_RCSD:							/**< 记录列选择描述符	*/
		case E_COMDCB:							/* 串口通信控制块 */
		{
			break;
		}
		
		default:
			dar = DATA_OTHER_REASON;
			break;
	}
		   
	   
	*convert->sOffset = sOffset;
	*convert->dOffset = dOffset;

	//ACUDP_FMT_TRACE("basic_data_to_fmt698() end: dOffset=%d, *convert->sOffset=%d, *convert->dOffset=%d, convert->srcLen=%d, convert->dstLen=%d \n", 
				//dOffset, *convert->sOffset, *convert->dOffset, convert->srcLen, convert->dstLen);
	
	return dar;
}


/**
*********************************************************************
* @brief：      数据中心格式转698规约格式
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
* @author    : 贺 宁
* @Date      ：2019-12-6               
*********************************************************************
*/
OOP_DAR_E basic_db_to_fmt698(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    DATA_E eData = 0;

     /*入参检查*/
    if ((pOadInfo == NULL) || (convert == NULL) || (convert->srcBuf == NULL) || 
        (convert->dstBuf == NULL)|| (convert->sOffset == NULL) || (convert->dOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_check failed. ERR_PTR\n");
        return ERR_PTR;
    }

    eData = pOadInfo->subeData;

    if(!basic_type_check(eData))
    {
        ACUDP_FMT_TRACE("basic_type_check failed. eData(%u)\n", eData);      
    }
    
    return basic_data_to_fmt698(eData, convert);  
}


/**
*********************************************************************
* @name：        basic_data_to_buf
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_data_to_buf(DATA_E eData, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;

    /*入参检查*/
    if ((srcData == NULL) || (dstBuf == NULL) || (pOffset == NULL) || (!basic_type_check(eData)))
    {
        ACUDP_FMT_TRACE("basic_data_to_buf check failed. ERR_PTR\n");
        return DATA_OTHER_REASON;
    }

    MEMZERO(&convert, sizeof(convert));
   
    convert.srcBuf = srcData;
    convert.srcLen = srcLen;
    convert.sOffset = &sOffset;;

    convert.dstBuf = dstBuf;
    convert.dstLen = dstLen;
    convert.dOffset = pOffset;

    dar = basic_data_to_fmt698(eData, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_from_fmt698 failed. eData(%u)\n", eData);
        return dar;
    }

    if(sOffset > srcLen)
    {
        ACUDP_FMT_TRACE("srcData cross the border. sOffset(%d), srcLen(%d)\n", sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }
    
    return dar;
}

/**
*********************************************************************
* @name：        apdu_ti_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_ti_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TI_T data; 
    
    MEMZERO(&data, sizeof(data));

    //DT_TI类型
    if(srcBufTmp[offset++] != DT_TI)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check ti type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    } 

    dar = apdu_ti_fromfmt698(srcBuf, srcLen, &offset, &data, sizeof(OOP_TI_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "apdu_ti_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }  

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}


/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_time_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_TIME_T data = {0}; 
    
    MEMZERO(&data, sizeof(data));

    //TIME类型
    if(srcBufTmp[offset++] != DT_TIME)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "check time type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }  

    data.hour =  srcBufTmp[offset++];
    data.minute =  srcBufTmp[offset++];
    data.second =  srcBufTmp[offset++];

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}

/**
*********************************************************************
* @name：        basic_rsd_sel9_fromfmt698
* @brief：       基本类型数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer

* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_datetime_s_fromfmt698(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;    
    uint8 *srcBufTmp = (uint8*)srcBuf;
    OOP_DATETIME_S_T data; 
    
    MEMZERO(&data, sizeof(data));

    //DATETIME_S类型
    if(srcBufTmp[offset++] != DT_DATETIME_S)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "check datetime_s type failed. offset(%d)", offset);
        return DATA_TYPE_UNMATCHED;
    }  

    dar = apdu_datetime_s_fromfmt698(srcBuf, srcLen, &offset, &data, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_DEBUG(srcBufTmp, srcLen, "apdu_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offset);
        return DATA_TYPE_UNMATCHED;           
    }   

    memcpy(dstData, &data, sizeof(data));

    *pOffset = offset;

    return dar;     

}


/**
*********************************************************************
* @name：        basic_buf_to_data
* @brief：       从698报文格式中获取基本类型数据
* @param[in] ： dstType    目的数据类型
*               srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的数据长度,通常是变量的大小
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
*               dstLen     目的长度，通常是变量的长度
* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_data_from_fmt698(DATA_E eData, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_OTHER_REASON;
    uint32  sOffset = 0;
    uint32  dOffset = 0;
    uint8 *srcBuf = (uint8 *)convert->srcBuf;    
    uint8 *dstBuf = (uint8 *)convert->dstBuf;
    uint32 srcLen = 0;
    uint32 dstLen = 0;   
    uint32 dataLen = 0;

     /*入参检查*/
    if ((convert == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL)|| 
        (convert->sOffset == NULL) || (convert->dOffset == NULL))
    {
        ACUDP_FMT_TRACE("basic_data_from_fmt698 check failed, ERR_PTR\n");
        return ERR_PTR;
    }

    if(!basic_type_check(eData))
    {
        ACUDP_FMT_TRACE("basic_type_check failed. eData(%u)\n", eData);      
    }
    
    sOffset = *convert->sOffset;
    dOffset = *convert->dOffset;
    srcLen = convert->srcLen;
    dstLen = convert->dstLen;

    switch(eData)
    {
        case E_NULL:                            /**< NULL               */
        {
            if (srcBuf[sOffset++] != DT_NULL)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            dar = ERR_OK;
            break;
        }

        case E_ARRAY:                           /**< vector             */
            break;

        case E_STRUCTURE:                       /**< struct             */
            break;

        case E_BOOL:                            /**< BOOLEAN               */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_BOOL)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint8));
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }
            
        case E_BIT_STRING:                      /**< CBitString         */
            break;

        case E_OOP_BITSTR8_T:                   /**< CBitString         */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_BIT_STRING)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            if (srcBuf[sOffset++] != 8)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }

            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint8));
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }

        case E_OOP_BITSTR64_T:                   /**< CBitString         */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < 8*sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_BIT_STRING)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            if (srcBuf[sOffset++] != 64)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }

            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], 8*sizeof(uint8));
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }

        case E_OOP_BITSTR128_T:                   /**< CBitString         */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < 16*sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_BIT_STRING)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            if (srcBuf[sOffset++] != 0x82)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }
            if (srcBuf[sOffset++] != 0x00)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }
            if (srcBuf[sOffset++] != 0x80)
            {
                dar = DATA_DATA_BLOCK_UNAVAILABLE;
                break;
            }
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], 16*sizeof(uint8));
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }

        case E_DOUBLE_LONG:                     /**< int                */
        {
            if (srcLen < 5)
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if((dstLen - dOffset) < sizeof(int32))
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if (srcBuf[sOffset++] != DT_DOUBLE_LONG)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(int32));
            sOffset += sizeof(int32);
            dOffset += sizeof(int32);
            
            dar = ERR_OK;
            break;
        }

        case E_DOUBLE_LONG_UNSIGNED:            /**< uint32             */
        {
            if (srcLen < 5)
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if((dstLen - dOffset) < sizeof(uint32))
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if (srcBuf[sOffset++] != DT_DOUBLE_LONG_UNSIGNED)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint32));
            sOffset += sizeof(uint32);
            dOffset += sizeof(uint32);

            dar = ERR_OK;
            break;
        }

        case E_OCTET_STRING:                    /**< COctetString       */
            break;
        case E_UTF8_STRING:                     /**< string             */
            break;
        case E_INTEGER:                         /**< char               */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(char))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_INTEGER)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(char));
            sOffset += sizeof(char);
            dOffset += sizeof(char);

            dar = ERR_OK;
            break;
        }

        case E_LONG:                            /**< int16              */
        {
            if (srcLen < 3)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(int16))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_LONG)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(int16));
            sOffset += sizeof(int16);
            dOffset += sizeof(int16);

            dar = ERR_OK;
            break;
        }

        case E_UNSIGNED:                        /**< uint8              */
        {
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_UNSIGNED)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint8));
            sOffset += sizeof(uint8);
            dOffset += sizeof(uint8);

            dar = ERR_OK;
            break;
        }

        case E_LONG_UNSIGNED:                   /**< uint16             */
        {
            if (srcLen < 3)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(uint16))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_LONG_UNSIGNED)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint16));
            sOffset += sizeof(uint16);
            dOffset += sizeof(uint16);

            dar = ERR_OK;
            break;
        }

        case E_LONG64:                          /**< int64              */
        {
            if (srcLen < 9)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(int64))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_LONG64)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(int64));
            sOffset += sizeof(int64);
            dOffset += sizeof(int64);

            dar = ERR_OK;
            break;
        }

        case E_LONG64_UNSIGNED:                 /**< uint64             */
        {
            if (srcLen < 9)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(uint64))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if (srcBuf[sOffset++] != DT_LONG64_UNSIGNED)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], sizeof(uint64));
            sOffset += sizeof(uint64);
            dOffset += sizeof(uint64);

            dar = ERR_OK;
            break;

        }

        case E_ENUM:                            /**< uint8              */
        {
            dataLen = sizeof(uint8);
            if (srcLen < 2)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(uint8))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_ENUM)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_FLOAT32:                         /**< float32            */
        {
            dataLen = sizeof(float32);
            if (srcLen < 5)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(float32))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_FLOAT32)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_FLOAT64:                         /**< float64            */
        {
            dataLen = sizeof(float64);
            if (srcLen < 9)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if ((dstLen - dOffset) < sizeof(float64))
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            
            if (srcBuf[sOffset++] != DT_FLOAT64)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }

        case E_DATETIME:                        /**< DateTime           */
            break;
        case E_DATE:                            /**< OOP_DATE_T         */
            break;
        case E_TIME:                            /**< OOP_TIME_T         */
        {
            dataLen = sizeof(OOP_TIME_T);
            dar = basic_time_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;
            break;
        }
            
        case E_DATETIME_S:                      /**< DateTime_S         */  
            dataLen = sizeof(OOP_DATETIME_S_T);
            dar = basic_datetime_s_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;                
            break;
        case E_OI:                              /**< OI                 */
        {
            dataLen = sizeof(OI);
            
            if (srcLen < sizeof(OI) + 1)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(OI))
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if (srcBuf[sOffset++] != DT_OI)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }
            
        case E_OAD:                             /**< OOP_OAD_U          */
        {
            dataLen = sizeof(OOP_OAD_U);
            
            if (srcLen < sizeof(OOP_OAD_U) + 1)
            {
                dar = DATA_OTHER_REASON;
                break;
            }
            if ((dstLen - dOffset) < sizeof(OOP_OAD_U))
            {
                dar = DATA_OTHER_REASON;
                break;
            }

            if (srcBuf[sOffset++] != DT_OAD)
            {
                dar = DATA_TYPE_UNMATCHED;
                break;
            }
            
            memcpy_rev(&dstBuf[dOffset], &srcBuf[sOffset], dataLen);
            sOffset += dataLen;
            dOffset += dataLen;

            dar = ERR_OK;
            break;
        }
            
        case E_ROAD:                            /**< OOP_ROAD_T         */
            break;
        case E_OMD:                             /**< OOP_OMD_U          */
            break;
        case E_TI:                              /**< 时间间隔           */
            dataLen = sizeof(OOP_TI_T);
            dar = basic_ti_fromfmt698(srcBuf, srcLen, &sOffset, dstBuf, dstLen);
            dOffset += dataLen;                               
            break;       
        default:
            dar = DATA_OTHER_REASON;
            break;
    }

    *convert->sOffset = sOffset;
    *convert->dOffset = dOffset;

    return dar;
}


/**
*********************************************************************
* @name：        basic_buf_to_data
* @brief：       从698报文格式中获取基本类型数据(用于目的数据区为变量的场景)
* @param[in] ： dstType    目的数据类型
*               srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的数据长度,通常是变量的大小
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
*               dstLen     目的长度，通常是变量的长度
* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E basic_buf_to_data(DATA_E eData, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    DATA_CONVERT_T convert;
    uint32 dOffset = 0;

    /*入参检查*/
    if ((srcBuf == NULL) || (dstData == NULL) || (pOffset == NULL) || (!basic_type_check(eData)))
    {
        ACUDP_FMT_TRACE("basic_buf_to_data check failed. ERR_PTR\n");
        return DATA_OTHER_REASON;
    }

    MEMZERO(&convert, sizeof(convert));
   
    convert.srcBuf = (void *)srcBuf;
    convert.srcLen = srcLen;
    convert.sOffset = pOffset;

    convert.dstBuf = dstData;
    convert.dstLen = dstLen;
    convert.dOffset = &dOffset;

    dar = basic_data_from_fmt698(eData, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_from_fmt698 failed. eData(%u)\n", eData);
        return dar;
    }

    if(dOffset > dstLen)
    {
        ACUDP_FMT_TRACE("dstData cross the border. dOffset(%d), dstLen(%d)\n", dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    return dar;
}

/**
*********************************************************************
* @brief：      可变数据的读取
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void datetime_s_data_to_buf(OOP_DATETIME_S_T tmVal, uint32 *offset, uint8 *apduValue)
{
    apduValue[(*offset)++] = (tmVal.year>>8)&0xff;
    apduValue[(*offset)++] = tmVal.year&0xff;
    apduValue[(*offset)++] = tmVal.month;  
    apduValue[(*offset)++] = tmVal.day;           
    apduValue[(*offset)++] = tmVal.hour; 
    apduValue[(*offset)++] = tmVal.minute; 
    apduValue[(*offset)++] = tmVal.second; 

    return;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(NULL)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_null(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    if (srcLen != 0)
    {
        ACUDP_FMT_DEBUG("data_to_buf_null srclen != 0\n");
        return DATA_TYPE_UNMATCHED;
    }

    //结构类型
    dstBuf[dOffset++] = DT_NULL;   

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @name：        normal_data_to_fmt698
* @brief：       非记录型的OAD结构数据转换为698报文格式
* @param[in] ： srcType    源数据类型
*               srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out]:  dOffset    构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
                sOffset    取出该数据后的源数据区偏移


* @return    ：无
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E normal_data_to_fmt698(const IC_OAD_INFO_T *pIcInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    OAD_INFO_T pstOdaInfo;
    uint32 i = 0;

    /*入参检查*/
    if ((pIcInfo == NULL) || (convert == NULL) || (convert->srcBuf == NULL) || (convert->dstBuf == NULL)|| 
        (convert->sOffset == NULL) || (convert->dOffset == NULL))
    {
        ACUDP_FMT_DEBUG("normal_data_to_fmt698 check failed. ERR_PTR\n");
        return ERR_PTR;
    }    

    MEMZERO(&pstOdaInfo, sizeof(OAD_INFO_T));   

    if(pIcInfo->choice == 0)
    {
        if(!class_oadinfo_get(pIcInfo->oad, &pstOdaInfo))
        {
            ACUDP_FMT_DEBUG("class_oadinfo_get failed.\n");
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }
    }
    else
    {
        memcpy(&pstOdaInfo, pIcInfo->pstOdaInfo, sizeof(OAD_INFO_T));
    }

    // 如果源数据长度为0，数据填NULL
    if (convert->srcLen == 0)
    {
        dar = data_to_buf_null(&pstOdaInfo, convert);
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_DEBUG(convert->srcBuf, convert->srcLen, "Data2Buf failed. dar(%d), class(%d), oad(0x%08x)\n", 
                dar, pstOdaInfo.pTab->class, pstOdaInfo.pTab->oad.value);
        }
        ACUDP_FMT_DEBUG("srcLen 0, data is null. oad(0x%08x)\n", pstOdaInfo.pTab->oad.value);
        return dar;
    }

    pstOdaInfo.subParse = pIcInfo->subParse;

    for(i = 0; i < OOP_IC_MAX; i++)
    {
        if(pstOdaInfo.pTab->class == g_ic_cov_op[i].ClassID)
        {
            dar = g_ic_cov_op[i].Data2Buf(&pstOdaInfo, convert);
            if(dar != DATA_SUCCESS)
            {
                ACUDP_BUF_DEBUG(convert->srcBuf, convert->srcLen, "Data2Buf failed. dar(%d), class(%d), oad(0x%08x)\n", 
                    dar, pstOdaInfo.pTab->class, pstOdaInfo.pTab->oad.value);
            }
            break;
        }
    }

    if(i == OOP_IC_MAX)
    {
        ACUDP_FMT_DEBUG("class%d: Data2Buf not support.\n", pstOdaInfo.pTab->class);
        dar =  DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    return dar;
}

/**
*********************************************************************
* @name：       all_rcsd_get
* @brief：      获取对应OAD的关联OAD
* @param[in]： dataType   数据类型
* @return    ：检查是否通过 1-通过，0-不通过
* @author    : 贺 宁
* @Date      ：2019-12-6
*********************************************************************
*/
int32 all_rcsd_get(OAD_INFO_T *pOadInfo, DB_CLIENT fd, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax)
{
    uint32 offset = *pOffset;

    if (pOadInfo->pTab == NULL)
    {
        ACUDP_FMT_DEBUG("all_rcsd_get pOadInfo->pTab[NULL]\n");

        //SEQUENCE OF CSD  
        pOutData[offset++] = 0;

        *pOffset = offset;

        return 0;
    }

    switch(pOadInfo->pTab->class)
    {
        case 7:
        {
            return evt_all_rcsd_get(pOadInfo, pOffset, pOutData, outDataMax);
        }break;
        case 9:
        {
            return frozen_all_rcsd_get(pOadInfo, fd, pOffset, pOutData, outDataMax);
        }break;

        default:break;
    }

    return 0;
}

OOP_DAR_E read_record_response(OAD_INFO_T *pOadInfo, DB_CLIENT fd, const uint8 *inData, uint16 InDataLen, uint16 offset, uint16 csdStart, RECORD_BUF_T *outRecordBuf, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outOffset = outData->resultDataLen;
    uint16 csdLen = 0;

    //为空封装返回报文头，非空拼装返回的数据
    if(outRecordBuf == NULL)
    { 
        /*填写OAD+RCSD+记录结果1 并记下偏移headLen*/
        csdLen = offset-csdStart;
        
        outOffset = 0; 
        memcpy(&outData->pResultData[outOffset], inData, sizeof(uint32));
        outOffset += sizeof(uint32);

        if(csdLen == 1) //RCSD=0
        {
            pOadInfo->noParse = FALSE;
            all_rcsd_get(pOadInfo, fd, &outOffset, outData->pResultData, outDataMax);
        }
        else
        {
            memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
            outOffset += csdLen;
        }
        
        outData->pResultData[outOffset++] = 1;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], outRecordBuf->recordBuf, outRecordBuf->bufLen);
        outOffset += outRecordBuf->bufLen;
    }

    outData->resultDataLen = outOffset;

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief：       召测普通OAD回复NULL类型数据
* @param[in] ：  
* @param[out]：  
* @return：      返回值
* @author:      贺宁
*********************************************************************
*/
OOP_DAR_E oop_null_data_res(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint32 offset = 0;

    //数据中心出错返回NULL
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset++] = 0x01;
    outData->pResultData[offset++] = DT_NULL;
    outData->resultDataLen += offset;
    
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：       data_type_length_get
* @brief：      数据单元长度
* @param[in] ： dataType       - 数据类型
* @param[out]：
* @return    ： 数据长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
uint8 data_type_length_get(enum tag_OOP_DATATYPE dataType)
{
    switch (dataType)
    {
    case DT_NULL:
        return 0;
    case DT_BOOL:
    case DT_UNSIGNED:
    case DT_INTEGER:
    case DT_ENUM:
        return 1;

    case DT_LONG:
    case DT_LONG_UNSIGNED:
    case DT_OI:
    case DT_SCALER_UNIT:
        return 2;

    case DT_TIME:
        return 3;
        //4
    case DT_DOUBLE_LONG:
    case DT_DOUBLE_LONG_UNSIGNED:

    case DT_FLOAT32:
    case DT_OAD:
    case DT_OMD:

        return 4;
        //5
    case DT_DATE:
    case DT_COMDCB:
        return 5;

        //7
    case DT_DATETIME_S:
        return 7;
        //8
    case DT_LONG64:

    case DT_LONG64_UNSIGNED:
    case DT_FLOAT64:
        return 8;
        //9

        //10
    case DT_DATETIME:
        return 10;
    default:
        return 0xFF;
    }
}

/**
*********************************************************************
* @name：       apdu_next_position_get
* @brief：      获取报文指定位置处，指定类型数据的长度，也即下一个数据位置
* @param[in] ：inData       输入报文
*              inDataLen   报文长度
*              DataType    需计算长度的数据类型
*              bWithType   1-数据类型由入参携带，0-数据类型从报文获取
* @param[out]：offset       输出的指定类型长度，即从inData的开始的DataType偏移量
* @return    ：查询结果 TRUE-成功，FALSE-失败
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
BOOLEAN apdu_next_position_get(const uint8 *inData, uint16 *offset, uint16 inDataLen, OOP_DATATYPE_E DataType, BOOLEAN bWithType)
{
    uint16 dataLen;
    uint8 nchioce;
    uint8 dataNum;
    uint8 i;
    uint16 offSet = *offset;
    OOP_DATATYPE_E dataType;

    if (bWithType)
    {
        dataType = DataType;
    }
    else
    {
        dataType = (OOP_DATATYPE_E)inData[offSet++];
    }

    if (dataType == DT_NULL)
    {
        *offset = offSet;
        return TRUE;
    }
    if ((dataType == DT_STRUCTURE)
        || (dataType == DT_ARRAY)
        || (dataType == DT_OCTET_STRING)
        || (dataType == DT_VISIBLE_STRING)
        || (dataType == DT_UTF8_STRING)
        || (dataType == DT_BIT_STRING)
        || (dataType == DT_MAC)
        || (dataType == DT_RN)
        || (dataType == DT_TSA))
    {
        dataLen = inData[offSet++];
        if (dataLen == 0x81)
            dataLen = inData[offSet++];
        else if (dataLen == 0x82)
        {
            dataLen = inData[offSet++];
            dataLen <<= 8;
            dataLen += inData[offSet++];
        }
        else if (dataLen > 0x82 || dataLen == 0x80)
        {
            return FALSE;
        }
        if ((dataType != DT_ARRAY) && (dataType != DT_OCTET_STRING) && (dataType != DT_VISIBLE_STRING) \
            && (dataType != DT_MAC) && (dataLen == 0))
        {
            return FALSE;
        }

        if (dataType == DT_BIT_STRING)
        {
            dataLen /= 8;
        }

        if ((dataType == DT_STRUCTURE)
            || (dataType == DT_ARRAY))
        {
            uint16 eleNum = dataLen;
            while (eleNum--)
            {
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
                {
                    return FALSE;
                }
            }
        }
        else
        {
            offSet += dataLen;
        }
    }
    else if (dataType == DT_RSD)
    {
        uint8 eleNum = 0;
        nchioce = inData[offSet++];
        switch (nchioce)
        {
        case 0://
            break;
        case 1:
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_OAD, TRUE))
                return FALSE;
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
                return FALSE;
            break;
        case 2:
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_OAD, TRUE))
                return FALSE;
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
                return FALSE;
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
                return FALSE;
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
                return FALSE;
            break;
        case 3:
            eleNum = inData[offSet++];
            for (i = 0; i < eleNum; i++)
            {
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_OAD, TRUE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
                    return FALSE;
                if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
                    return FALSE;
            }
            break;
        case 4:
        case 5:
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                return FALSE;
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_MS, TRUE))
                return FALSE;
            break;
        case 6:
        case 7:
        case 8:
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                return FALSE;
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_DATETIME_S, TRUE))
                return FALSE;
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_TI, TRUE))
                return FALSE;
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_MS, TRUE))
                return FALSE;
            break;
        case 9:
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_UNSIGNED, TRUE))
                return FALSE;
            break;
        case 10:
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_UNSIGNED, TRUE))
                return FALSE;
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_MS, TRUE))
                return FALSE;
            break;

        }
    }
    else if (dataType == DT_CSD)
    {
        nchioce = inData[offSet++];
        if (nchioce == 0)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet += dataLen;
        }
        else if (nchioce == 1)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet += dataLen;
            dataNum = inData[offSet++];
            offSet += dataLen * dataNum;
        }
        else
            return FALSE;

    }
    else if (dataType == DT_ROAD)
    {
        dataLen = data_type_length_get(DT_OAD);
        offSet += dataLen;
        dataNum = inData[offSet++];
        offSet += dataLen * dataNum;
        printf("ROAD LEN = %d\n", dataLen + 1 + dataLen * dataNum);
    }
    else if (dataType == DT_RCSD)
    {
        dataNum = inData[offSet++];
        for (i = 0; i < dataNum; i++)
        {
            if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_CSD, TRUE))
                return FALSE;
        }
    }
    else if (dataType == DT_MS)
    {
        nchioce = inData[offSet++];
        if ((nchioce == 0) || (nchioce == 1))
        {

        }
        else if (nchioce == 2)//一组用户类型
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_UNSIGNED);
            offSet += dataLen * dataNum;
        }
        else if (nchioce == 3)
        {
            dataNum = inData[offSet++];
            while (dataNum--)
            {
                dataLen = inData[offSet++];
                if ((dataLen > 8) || (dataLen == 0))
                    return FALSE;
                offSet += dataLen;
            }
        }
        else if (nchioce == 4)
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_LONG_UNSIGNED);
            offSet += dataLen * dataNum;
        }
        else if (nchioce == 5)//一组用户类型区间
        {
            dataNum = inData[offSet++];

            while (dataNum--)
            {
                if (inData[offSet++] > 3)
                    return FALSE;
                if (inData[offSet++] != DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet += dataLen;
                if (inData[offSet++] != DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet += dataLen;
            }
        }
        else if (nchioce == 6)//一组用户地址区间
        {
            dataNum = inData[offSet++];
            while (dataNum--)
            {
                if (inData[offSet++] > 3)
                    return FALSE;
                if (inData[offSet++] != DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if ((dataLen > 8) || (dataLen == 0))
                    return FALSE;
                offSet += dataLen;
                if (inData[offSet++] != DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if ((dataLen > 8) || (dataLen == 0))
                    return FALSE;
                offSet += dataLen;
            }
        }
        else if (nchioce == 7)//一组配置序号区间
        {
            dataNum = inData[offSet++];

            while (dataNum--)
            {
                if (inData[offSet++] > 3)
                    return FALSE;
                if (inData[offSet++] != DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet += dataLen;
                if (inData[offSet++] != DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet += dataLen;
            }
        }
        else
            return FALSE;

    }
    else if (dataType == DT_REGION)
    {
        if (inData[offSet++] > 3)
            return FALSE;
        if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
            return FALSE;
        if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_NULL, FALSE))
            return FALSE;
    }
    else if (dataType == DT_SID)
    {
        if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_DOUBLE_LONG_UNSIGNED, TRUE))
            return FALSE;
        if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_OCTET_STRING, TRUE))
            return FALSE;
    }
    else if (dataType == DT_SIDMAC)
    {
        if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_SID, TRUE))
            return FALSE;
        if (FALSE == apdu_next_position_get(inData, &offSet, inDataLen, DT_MAC, TRUE))
            return FALSE;
    }
    else if (dataType == DT_TI)
    {
        if (inData[offSet++] > 5)
            return FALSE;
        offSet += 2;
    }
    else
    {

        dataLen = data_type_length_get(dataType);
        if (0xFF != dataLen)
            offSet += dataLen;
        else
            return FALSE;
    }
    if (offSet > inDataLen)
        return FALSE;
    *offset = offSet;
    return TRUE;
}

/****************************************************************************
*函数名称：taskmng_oop_data_extract
*功能：     将面向对象报文转换成结构体
*输入参数：
*输出参数：
*返回：
****************************************************************************/
BOOL  taskmng_oop_data_extract(const uint8* inData, uint16* offset, uint16 inDataLen, uint8* tembuffer, enum tag_OOP_DATATYPE DataType, BOOL bWithType)
{
    uint16 dataLen;
    uint8 nchioce;
    uint8 dataNum;
    uint8 i;
    uint16 offSet = *offset;
    enum tag_OOP_DATATYPE dataType;

    if (bWithType)
    {
        dataType = DataType;
    }
    else
    {
        dataType = (enum tag_OOP_DATATYPE)inData[offSet++];
    }

    if (dataType == DT_NULL)
    {
        *offset = offSet;
        return TRUE;
    }
    if ((dataType == DT_STRUCTURE)
        || (dataType == DT_ARRAY)
        || (dataType == DT_OCTET_STRING)
        || (dataType == DT_VISIBLE_STRING)
        || (dataType == DT_UTF8_STRING)
        || (dataType == DT_BIT_STRING)
        || (dataType == DT_MAC)
        || (dataType == DT_RN)
        || (dataType == DT_TSA))
    {
        dataLen = inData[offSet++];
        if (dataLen == 0x81)
            dataLen = inData[offSet++];
        else if (dataLen == 0x82)
        {
            dataLen = inData[offSet++];
            dataLen <<= 8;
            dataLen += inData[offSet++];
        }
        else if (dataLen > 0x82 || dataLen == 0x80)
        {
            return FALSE;
        }
        if ((dataType != DT_ARRAY) && (dataType != DT_OCTET_STRING) && (dataType != DT_VISIBLE_STRING) \
            && (dataType != DT_MAC) && (dataLen == 0))
        {
            return FALSE;
        }

        if (dataType == DT_BIT_STRING)
        {
            if (dataLen % 8 == 0)
                dataLen /= 8;
            else
                dataLen = (dataLen / 8 + 1);
        }

        if ((dataType == DT_STRUCTURE)
            || (dataType == DT_ARRAY))
        {
            //uint16 eleNum = dataLen;

            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
            {
                return FALSE;
            }
        }
        else
        {
            memcpy(tembuffer, inData + offSet, dataLen);
            offSet += dataLen;
        }
    }
    else if (dataType == DT_RSD)
    {
        uint8 eleNum = 0;
        nchioce = inData[offSet++];
        switch (nchioce)
        {
        case 0://
            break;
        case 1:
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_OAD, TRUE))
                return FALSE;
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
                return FALSE;
            break;
        case 2:
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_OAD, TRUE))
                return FALSE;
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
                return FALSE;
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
                return FALSE;
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
                return FALSE;
            break;
        case 3:
            eleNum = inData[offSet++];    //SEQUENCE OF Selector2
            for (i = 0; i < eleNum; i++)
            {
                if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_OAD, TRUE))
                    return FALSE;
                if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
                    return FALSE;
                if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
                    return FALSE;
                if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
                    return FALSE;
            }
            break;
        case 4:
        case 5:
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_DATETIME_S, TRUE))
                return FALSE;
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_MS, TRUE))
                return FALSE;
            break;
        case 6:
        case 7:
        case 8:
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_DATETIME_S, TRUE))
                return FALSE;
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_DATETIME_S, TRUE))
                return FALSE;
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_TI, TRUE))
                return FALSE;
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_MS, TRUE))
                return FALSE;
            break;
        case 9:
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_UNSIGNED, TRUE))
                return FALSE;
            break;
        case 10:
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_UNSIGNED, TRUE))
                return FALSE;
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_MS, TRUE))
                return FALSE;
            break;

        }
    }
    else if (dataType == DT_CSD)
    {
        nchioce = inData[offSet++];
        if (nchioce == 0)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet += dataLen;
        }
        else if (nchioce == 1)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet += dataLen;
            dataNum = inData[offSet++];
            offSet += dataLen * dataNum;
        }
        else
            return FALSE;

    }
    else if (dataType == DT_ROAD)
    {
        dataLen = data_type_length_get(DT_OAD);
        offSet += dataLen;
        dataNum = inData[offSet++];
        offSet += dataLen * dataNum;
        printf("ROAD LEN = %d\n", dataLen + 1 + dataLen * dataNum);
    }
    else if (dataType == DT_RCSD)
    {
        dataNum = inData[offSet++];
        for (i = 0; i < dataNum; i++)
        {
            if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_CSD, TRUE))
                return FALSE;
        }
    }
    else if (dataType == DT_MS)
    {
        nchioce = inData[offSet++];
        if ((nchioce == 0) || (nchioce == 1))
        {

        }
        else if (nchioce == 2)//一组用户类型
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_UNSIGNED);
            offSet += dataLen * dataNum;
        }
        else if (nchioce == 3)
        {
            dataNum = inData[offSet++];
            while (dataNum--)
            {
                dataLen = inData[offSet++];
                if ((dataLen > 8) || (dataLen == 0))
                    return FALSE;
                offSet += dataLen;
            }
        }
        else if (nchioce == 4)
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_LONG_UNSIGNED);
            offSet += dataLen * dataNum;
        }
        else if (nchioce == 5)//一组用户类型区间
        {
            dataNum = inData[offSet++];

            while (dataNum--)
            {
                if (inData[offSet++] > 3)
                    return FALSE;
                if (inData[offSet++] != DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet += dataLen;
                if (inData[offSet++] != DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet += dataLen;
            }
        }
        else if (nchioce == 6)//一组用户地址区间
        {
            dataNum = inData[offSet++];
            while (dataNum--)
            {
                if (inData[offSet++] > 3)
                    return FALSE;
                if (inData[offSet++] != DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if ((dataLen > 8) || (dataLen == 0))
                    return FALSE;
                offSet += dataLen;
                if (inData[offSet++] != DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if ((dataLen > 8) || (dataLen == 0))
                    return FALSE;
                offSet += dataLen;
            }
        }
        else if (nchioce == 7)//一组配置序号区间
        {
            dataNum = inData[offSet++];

            while (dataNum--)
            {
                if (inData[offSet++] > 3)
                    return FALSE;
                if (inData[offSet++] != DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet += dataLen;
                if (inData[offSet++] != DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet += dataLen;
            }
        }
        else
            return FALSE;

    }
    else if (dataType == DT_REGION)
    {
        if (inData[offSet++] > 3)
            return FALSE;
        if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
            return FALSE;
        if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_NULL, FALSE))
            return FALSE;
    }
    else if (dataType == DT_SID)
    {
        if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DOUBLE_LONG_UNSIGNED, TRUE))
            return FALSE;
        if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_OCTET_STRING, TRUE))
            return FALSE;
    }
    else if (dataType == DT_SIDMAC)
    {
        if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_SID, TRUE))
            return FALSE;
        if (FALSE == taskmng_oop_data_extract(inData, &offSet, inDataLen, tembuffer, DT_MAC, TRUE))
            return FALSE;
    }
    else if (dataType == DT_TI)
    {
        if (inData[offSet++] > 5)
            return FALSE;
        offSet += 2;
    }
    else
    {
        //printf("AAAAAAAAAAAAAAdataType is:%d\n",dataType);
        dataLen = data_type_length_get(dataType);
        memcpy(tembuffer, inData + offSet, dataLen);
        //printf("taskmng_oop_data_extract dataLen is:%d\n",dataLen);
        //printf("taskmng_oop_data_extract offSet is:%d\n",offSet);
        if (0xFF != dataLen)
            offSet += dataLen;
        else
            return FALSE;
    }
    //printf("taskmng_oop_data_extract offSet is:%d\n",offSet);
    //printf("taskmng_oop_data_extract inDataLen is:%d\n",inDataLen);
    if (offSet > inDataLen)
        return FALSE;
    *offset = offSet;
    return TRUE;
}


