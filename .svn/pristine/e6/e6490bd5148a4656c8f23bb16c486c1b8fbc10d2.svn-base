/*
*********************************************************************
* @file    class6.c
* @brief： class6处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"


//
// class6
//

#if DESC("基础单元D2B",1)

/*********************************************************************
* @brief：      电压合格率
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_voltqr_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_VOLTQR_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 5;
            
    //电压监测时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwTime1, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),dwTime1(%d)\n", dar, data.dwTime1);
        return DATA_TYPE_UNMATCHED;
    }
    
    //电压合格率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wRate1, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),wRate1(%d)\n", dar, data.wRate1);
        return DATA_TYPE_UNMATCHED;
    }

    //电压超限率
    dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.wRate2, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),wRate2(%d)\n", dar, data.wRate2);
        return DATA_TYPE_UNMATCHED;
    }        

    //电压超上限时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwTime3, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),dwTime3(%d)\n", dar, data.dwTime3);
        return DATA_TYPE_UNMATCHED;
    }    

    //电压超下限时间
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.dwTime4, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_data_to_buf failed. dar(%d),dwTime4(%d)\n", dar, data.dwTime4);
        return DATA_TYPE_UNMATCHED;
    }        

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/*********************************************************************
* @brief：      电压合格率
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_voltqr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_VOLTSTAT_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    DATA_CONVERT_T unitConvert;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    memcpy(&unitConvert, convert, sizeof(DATA_CONVERT_T));

    //结构类型
    dstBuf[dOffset++] = DT_STRUCTURE;

    //结构成员数
    dstBuf[dOffset++] = 2;
    
    unitConvert.dOffset = &dOffset;

    *unitConvert.sOffset = 0;
    unitConvert.srcBuf = &data.day;
    unitConvert.srcLen = sizeof(OOP_VOLTQR_T);

    dar = data_to_buf_voltqr_unit(pOadInfo, &unitConvert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&srcBuf[sOffset], srcLen-sOffset, "data_to_buf_voltqr_unit day failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }
    
    *unitConvert.sOffset = 0;
    unitConvert.srcBuf = &data.mon;

    dar = data_to_buf_voltqr_unit(pOadInfo, &unitConvert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&srcBuf[sOffset], srcLen-sOffset, "data_to_buf_voltqr_unit mon failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

#endif

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class6)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class6(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_class6() 入参检查 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
        case E_LONG_UNSIGNED:
        {
        	ACUDP_FMT_TRACE("data_to_buf_class6() case E_LONG_UNSIGNED: \n");
            uint16 data[2];
            if (dsdata->srcLen != sizeof(uint16))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, data, sizeof(uint16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
        	break;
        case E_LONG:
        {
        	ACUDP_FMT_TRACE("data_to_buf_class6() case E_LONG: \n");
            int16 data[2];
            if (dsdata->srcLen != sizeof(uint16))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, data, sizeof(int16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
        	break;
        case E_DOUBLE_LONG_UNSIGNED:
        {
            ACUDP_FMT_TRACE("data_to_buf_class6() case E_DOUBLE_LONG_UNSIGNED: \n");
        	uint32 data[4];
            if (dsdata->srcLen != sizeof(uint32))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset, data, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
        	break;
        case E_DOUBLE_LONG:
        {
            ACUDP_FMT_TRACE("data_to_buf_class6() case E_DOUBLE_LONG: \n");
            int32 data[4];
            if (dsdata->srcLen != sizeof(int32))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy(data, srcDataTmp, dsdata->srcLen); 
            if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen-offset, &offset, data, sizeof(int32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            nRet = ERR_OK;
         
        }
        	break;
        case E_OOP_VOLTSTAT_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class6() case E_OOP_VOLTSTAT_T: \n");
            OOP_VOLTSTAT_T VOLTSTAT_T;

            //不解析，直接返回数据
            if(pOadInfo->noParse)
            {
                memcpy((uint8*)&VOLTSTAT_T, srcDataTmp, dsdata->srcLen); 
                switch(pOadInfo->nIndex)
                {
                    case 0: 
                    {
                        memcpy(dstBufTmp, &VOLTSTAT_T, sizeof(VOLTSTAT_T)); 
                        offset += sizeof(VOLTSTAT_T);
                    }break;

                    case 1: 
                    {
                        memcpy(dstBufTmp, &VOLTSTAT_T.day, sizeof(OOP_VOLTQR_T)); 
                        offset += sizeof(OOP_VOLTQR_T);
                    }break;
                    case 2: 
                    {
                        memcpy(dstBufTmp, &VOLTSTAT_T.mon, sizeof(OOP_VOLTQR_T)); 
                        offset += sizeof(OOP_VOLTQR_T);
                    }break;  
                }
            }
            else
            {
                switch(pOadInfo->nIndex)
                {
                    case 0:
                    {
                        nRet = data_to_buf_voltqr(pOadInfo, dsdata);
                        if(nRet != ERR_OK)
                        {
                            ACUDP_FMT_TRACE("data_to_buf_voltqr failed. nRet(%d)\n", nRet);
                            nRet = ERR_NORMAL;
                            return nRet;
                        }
                    }break;

                    case 1:
                    case 2:    
                    {
                        nRet = data_to_buf_voltqr_unit(pOadInfo, dsdata);
                        if(nRet != ERR_OK)
                        {
                            ACUDP_FMT_TRACE("data_to_buf_voltqr failed. nRet(%d)\n", nRet);
                            nRet = ERR_NORMAL;
                            return nRet;
                        }
                    }break;                            
                }

                offset = *dsdata->dOffset;
            }
            
            nRet = ERR_OK;
         
        }
        	break;
        case E_OOP_DWORD2_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class6() case E_OOP_DWORD2_T: \n");
            OOP_DWORD2_T DWORD2_T;
            if (dsdata->srcLen != sizeof(OOP_DWORD2_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy((uint8*)&DWORD2_T, srcDataTmp, dsdata->srcLen); 
            dstBufTmp[offset++]= DT_STRUCTURE;
            dstBufTmp[offset++]= 0x02;

            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&DWORD2_T.nValue1, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&DWORD2_T.nValue2, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            nRet = ERR_OK;
         
        }
        	break;
        case E_OOP_WORD2_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class6() case E_OOP_WORD2_T: \n");
            OOP_WORD2_T WORD2_T;
            if (dsdata->srcLen != sizeof(OOP_WORD2_T))
            {
                nRet = ERR_OPTION;
                break;
            }
            memcpy((uint8*)&WORD2_T, srcDataTmp, dsdata->srcLen); 
            ACUDP_FMT_LOGCC("WORD2_T.nValue1 = %d\n", WORD2_T.nValue1);
            ACUDP_FMT_LOGCC("WORD2_T.nValue2 = %d\n", WORD2_T.nValue2);
            dstBufTmp[offset++]= DT_STRUCTURE;
            dstBufTmp[offset++]= 0x02;

            if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&WORD2_T.nValue1, sizeof(uint16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dsdata->dstLen-offset, &offset,(uint8*)&WORD2_T.nValue2, sizeof(uint16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }

            nRet = ERR_OK;
         
        }
        	break;
        case E_OOP_METWORD_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class6() case E_OOP_METWORD_T: \n");
            ARRAY_INFO_T arrayInfo;
            OOP_METWORD_T metword = {0};

            MEMZERO(&arrayInfo, sizeof(arrayInfo));

            //传进来的srcLen可能只是数组成员的长度,会比结构长度小，所以只有等于结构长度才拷贝
            if (dsdata->srcLen == sizeof(OOP_METWORD_T))
            {
                memcpy(&metword, srcDataTmp, dsdata->srcLen); 
            }
            
            arrayInfo.arrLen = sizeof(OOP_METWORD_T);
            arrayInfo.pArray = metword.metWord;
            arrayInfo.maxNum = OOP_METWORD_NUM;
            arrayInfo.unitNum = metword.nNum;
            arrayInfo.unitSize = sizeof(uint16);
            arrayInfo.func = basic_db_to_fmt698;

            //dsdata->dOffset = &offset;  //错误:修改了dsdata->dOffset的值
            
            pOadInfo->subeData = E_OOP_BITSTR16_T; //重要，基本数据一定要填子结构的数据类型
            
            nRet = class_array_tofmt698(pOadInfo, &arrayInfo, dsdata);
			return nRet;

        }
			break;
        
	    default: ACUDP_FMT_TRACE("data_to_buf_class6 failed.unknow eData(%d)\n", pOadInfoTmp->pTab->eData);
	        nRet = ERR_OPTION;
	        break;
    }

    *dsdata->dOffset = offset;

    return nRet;
}

OOP_DAR_E buf_to_data_class6(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}


