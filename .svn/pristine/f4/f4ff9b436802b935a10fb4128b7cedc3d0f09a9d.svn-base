/*
*********************************************************************
* @file    class1.c
* @brief： class1处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"

//
// class1
//


/**
*********************************************************************
* @brief：     698格式报文转从数据中心的数据(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_energy(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint32 tmpdOffset=0;
    OOP_ENERGY_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint8 nindex=0;
    uint8 num=0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);
    
    if(pOadInfo->nIndex==0)
    {
        //结构类型
        dstBuf[dOffset++] = DT_ARRAY;

        //结构成员数
        tmpdOffset = dOffset;
        dstBuf[dOffset++] = data.nNum;

        for (i = 0; i < data.nNum; i++)
        {
            if(0==get_bit_value((uint8*)&data.rsv,32,i))
            {
                //dstBuf[dOffset++] = DT_NULL;
                continue;
            }
            dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(uint32));
            if(dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            }  
            num++;
        }  
        dstBuf[tmpdOffset] =num;
    }
    else
    {
        if(pOadInfo->nIndex>OOP_MAX_RATES)
        {
            return ERR_NORMAL;
        }
        nindex=pOadInfo->nIndex-1;
        if(0==get_bit_value((uint8*)&data.rsv,32,nindex))
        {
            dstBuf[dOffset++] = DT_NULL;

        }
        else
        {
            dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[nindex], sizeof(uint32));
            if (dar != DATA_SUCCESS)
            {
                PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
                return DATA_TYPE_UNMATCHED;
            } 
        }

    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_energys(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint32 tmpdOffset;
    OOP_ENERGYS_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint8 nindex=0;
    uint8 num=0;
    
    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    if(pOadInfo->nIndex==0)
    {
        //结构类型
        dstBuf[dOffset++] = DT_ARRAY;

        //结构成员数
        tmpdOffset = dOffset;
        dstBuf[dOffset++] = data.nNum;

        for (i = 0; i < data.nNum; i++)
        {
            if(0==get_bit_value((uint8*)&data.rsv,32,i))
            {
                //dstBuf[dOffset++] = DT_NULL;
                continue;
            }
            dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(int32));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
                return DATA_TYPE_UNMATCHED;
            } 
            num++;
        } 
        dstBuf[tmpdOffset] = num;
    }
    else
    {
        if(pOadInfo->nIndex>OOP_MAX_RATES)
        {
            return ERR_NORMAL;
        }
        nindex=pOadInfo->nIndex-1;
        if(0==get_bit_value((uint8*)&data.rsv,32,nindex))
        {
            dstBuf[dOffset++] = DT_NULL;
        }
        else
        {
            dar = basic_data_to_buf(E_DOUBLE_LONG, dstBuf, dstLen, &dOffset, &data.nValue[nindex], sizeof(int32));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
                return DATA_TYPE_UNMATCHED;
            }  
        }

    }
    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_henergy(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint32  tmpdOffset=0;
    OOP_HENERGY_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint8 nindex=0;
    uint8 num=0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    if(pOadInfo->nIndex==0)
    {
        //结构类型
        dstBuf[dOffset++] = DT_ARRAY;

        //结构成员数
        tmpdOffset = dOffset;
        dstBuf[dOffset++] = data.nNum;

        for (i = 0; i < data.nNum; i++)
        {
            if(0==get_bit_value((uint8*)&data.rsv,32,i))
            {
                //dstBuf[dOffset++] = DT_NULL;
                continue;
            }
            dar = basic_data_to_buf(E_LONG64_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(uint64));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
                return DATA_TYPE_UNMATCHED;
            }
            num++;
        }  
        dstBuf[tmpdOffset] =num;
    }
    else
    {
        if(pOadInfo->nIndex>OOP_MAX_RATES)
        {
            return ERR_NORMAL;
        }
        nindex=pOadInfo->nIndex-1;
        if(0==get_bit_value((uint8*)&data.rsv,32,nindex))
        {
            dstBuf[dOffset++] = DT_NULL;
            
        }
        else
        {
            dar = basic_data_to_buf(E_LONG64_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[nindex], sizeof(uint64));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
                return DATA_TYPE_UNMATCHED;
            }  
        }
        

    }
    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}



/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1_henergys(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint32  tmpdOffset=0;
    OOP_HENERGYS_T data = {0};
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint8 nindex=0;
    uint8 num=0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    if(pOadInfo->nIndex==0)
    {
        //结构类型
        dstBuf[dOffset++] = DT_ARRAY;

        //结构成员数
        tmpdOffset = dOffset;
        dstBuf[dOffset++] = data.nNum;

        for (i = 0; i < data.nNum; i++)
        {
            if(0==get_bit_value((uint8*)&data.rsv,32,i))
            {
                //dstBuf[dOffset++] = DT_NULL;
				continue;
            }
            dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.nValue[i], sizeof(int64));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
                return DATA_TYPE_UNMATCHED;
            }   
            num++;
        } 
        dstBuf[tmpdOffset] =num;
    }
    else
    {
        if(pOadInfo->nIndex>OOP_MAX_RATES)
        {
            return ERR_NORMAL;
        }
        nindex=pOadInfo->nIndex-1;
        if(0==get_bit_value((uint8*)&data.rsv,32,nindex))
        {
            dstBuf[dOffset++] = DT_NULL;
			//continue;
            
        }
        else
        {
            dar = basic_data_to_buf(E_LONG64_UNSIGNED, dstBuf, dstLen, &dOffset, &data.nValue[nindex], sizeof(int64));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),data.nValue(%d)\n", i, dar, data.nValue[i]);
                return DATA_TYPE_UNMATCHED;
            }  
        }

    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}



/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class1)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->eData)
    {
        case E_OOP_ENERGY_T:   //电能量(不带符号)
        {
           return data_to_buf_class1_energy(pOadInfo,  convert);
        }break;

        case E_OOP_ENERGYS_T:   //电能量(带符号)
        {
           return data_to_buf_class1_energys(pOadInfo,  convert);
        }break;

        case E_OOP_HENERGY_T:   //高精度电能量(不带符号)
        {
           return data_to_buf_class1_henergy(pOadInfo,  convert);
        }break;

        case E_OOP_HENERGYS_T:   //高精度电能量(带符号)
        {
           return data_to_buf_class1_henergys(pOadInfo,  convert);
        }break;

        //......
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：       class1_invoke_get
* @brief：      class1的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_get_class1_energy(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0; 
    uint32 i = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->eData)
    {
    case E_OOP_ENERGY_T:
        {
            OOP_ENERGY_T data = {0};
            normal.logicId = 0x01;
            normal.infoNum = 0x00;
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.oad.nIndex = 0;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);

                return oop_null_array_res(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            }

            PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nValue[%d] = %d\n", i, data.nValue[i]);
            }

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class1(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class1 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：       class1_invoke_get
* @brief：      class1的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_get_class1_energys(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0; 
    uint32 i = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->eData)
    {
    case E_OOP_ENERGYS_T:
        {
            OOP_ENERGYS_T data = {0};
            normal.logicId = 0x01;
            normal.infoNum = 0x00;
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.oad.nIndex = 0;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                
                return oop_null_array_res(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            }

            PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nValue[%d] = %d\n", i, data.nValue[i]);
            }

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class1(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class1 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：       class1_invoke_get
* @brief：      class1的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_get_class1_henergy(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0; 
    uint32 i = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->eData)
    {
    case E_OOP_HENERGY_T:
        {
            OOP_HENERGY_T data = {0};
            normal.logicId = 0x01;
            normal.infoNum = 0x00;
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.oad.nIndex = 0;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                
                return oop_null_array_res(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            }

            PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nValue[%d] = %d\n", i, data.nValue[i]);
            }

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class1(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class1 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name：       class1_invoke_get
* @brief：      class1的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_get_class1_henergys(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0; 
    uint32 i = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    switch (pOadInfo->pTab->eData)
    {
    case E_OOP_HENERGYS_T:
        {
            OOP_HENERGYS_T data = {0};
            normal.logicId = 0x01;
            normal.infoNum = 0x00;
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.oad.nIndex = 0;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);

                //数据中心出错返回NULL
                return oop_null_array_res(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            }

            PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nValue[%d] = %d\n", i, data.nValue[i]);
            }

            //转换成报文数据
            convert.srcBuf = &data;
            convert.srcLen = sizeof(data);
            convert.sOffset = &sOffset;
            
            convert.dstBuf = &outData->pResultData[5];
            convert.dstLen = outDataMax;
            convert.dOffset = &outData->resultDataLen;
        }
        break;
    
    default:
        break;
    }
    
    /* 数据和报文转换 */
    dar = data_to_buf_class1(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class1 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}


#if DESC("class1对外接口函数",1)

/**
*********************************************************************
* @name：       class1_invoke_set
* @brief：      class1的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name：       class1_invoke_get
* @brief：      class1的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->eData)
    {
        case E_OOP_ENERGY_T:   //电能量(不带符号)
        {
           return class1_invoke_get_class1_energy(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case E_OOP_ENERGYS_T:   //电能量(带符号)
        {
           return class1_invoke_get_class1_energys(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case E_OOP_HENERGY_T:   //高精度电能量(不带符号)
        {
           return class1_invoke_get_class1_henergy(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case E_OOP_HENERGYS_T:   //高精度电能量(带符号)
        {
           return class1_invoke_get_class1_henergys(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        //......
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name：       class1_invoke_act
* @brief：      class1的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
