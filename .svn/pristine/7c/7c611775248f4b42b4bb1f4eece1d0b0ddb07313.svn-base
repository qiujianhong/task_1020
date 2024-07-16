/*
*********************************************************************
* @file    class4.c
* @brief： class4处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"
#include "ac_sample.h"


//
// class4
//

/**
*********************************************************************
* @brief：      class4 分项数据
* @param[in] ： 无
* @param[out]   pDesbuffer 原始数据结构体
*               nDesLen    原始数据长度
*               OAD        分项数据标识
* @return：     >0            - 成功
               <0            - 错误码
*********************************************************************
*/
int combine_sub_data_class4_buff(uint8 *pDesbuffer, uint16 *nDesLen, uint32 OAD)
{
    switch(OAD)
    {
        /*class 4*/
        case 0x200a0201:               //交采总功率因数
        case 0x200a0202:               //交采A相总功率因数
        case 0x200a0203:               //交采B相总功率因数
        case 0x200a0204:               //交采C相总功率因数
        {
            uint8 subLabel = OAD & 0x0000000f;
            int16 nValue = 0;

            if(subLabel == 1)
			{
				if(gMidPriMeasurePara.PowerFactorType.AppPfFlag == 1)//1负0正
				{
					nValue = 0 - MidTermSharedData.PowerFactor;
				}
				else
				{
					nValue = MidTermSharedData.PowerFactor;
				}
			}
			else if(subLabel == 2)
			{
				if(gMidPriMeasurePara.PowerFactorType.PhaAAppPfFlag == 1)//1负0正
				{
					nValue = 0 - MidTermSharedData.PhaAPowerFactor;
				}
				else
				{
					nValue = MidTermSharedData.PhaAPowerFactor;
				}
			}
			else if(subLabel == 3)
			{
				if(gMidPriMeasurePara.PowerFactorType.PhaBAppPfFlag == 1)//1负0正
				{
					nValue = 0 - MidTermSharedData.PhaBPowerFactor;
				}
				else
				{
					nValue = MidTermSharedData.PhaBPowerFactor;
				}
			}
			else if(subLabel == 4)
			{
				if(gMidPriMeasurePara.PowerFactorType.PhaCAppPfFlag == 1)//1负0正
				{
					nValue = 0 - MidTermSharedData.PhaCPowerFactor;
				}
				else
				{
					nValue = MidTermSharedData.PhaCPowerFactor;
				}
			}
            
			*nDesLen = sizeof(int16);
       		memcpy(pDesbuffer, &nValue, sizeof(int16));
        }
            break;

        default:
			return -1;
            break;
    }

    return 1;
}

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class4)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class4_base_data(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
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
        ACUDP_FMT_TRACE("data_to_buf_class4_base_data() 入参检查 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
        case E_LONG_UNSIGNED:
        {
            ACUDP_FMT_TRACE("data_to_buf_class4_base_data() case E_LONG_UNSIGNED: \n");
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
            ACUDP_FMT_TRACE("data_to_buf_class4_base_data() case E_LONG: \n");
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
            ACUDP_FMT_TRACE("data_to_buf_class4_base_data() case E_DOUBLE_LONG_UNSIGNED: \n");
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
            ACUDP_FMT_TRACE("data_to_buf_class4_base_data() case E_DOUBLE_LONG: \n");
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
        case E_OOP_DWORD2_T:
        {
            ACUDP_FMT_TRACE("data_to_buf_class4_base_data() case E_OOP_DWORD2_T: \n");
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
            ACUDP_FMT_TRACE("data_to_buf_class4_base_data() case E_OOP_WORD2_T: \n");
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
            ACUDP_FMT_TRACE("data_to_buf_class4_base_data() case E_OOP_METWORD_T: \n");
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
        
        default: ACUDP_FMT_TRACE("data_to_buf_class4_base_data failed.unknow eData(%d)\n", pOadInfoTmp->pTab->eData);
            nRet = ERR_OPTION;
            break;
    }

    *dsdata->dOffset = offset;

    return nRet;
}


/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文(class4)
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class4(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata)
{
    int32 nRet = ERR_NORMAL;
    const OAD_INFO_T *pOadInfoTmp = NULL;
    uint32     offset = *dsdata->dOffset;
    uint8      offlen = 0;
    uint8      lenBuf[3] = {0};
    uint32     i = 0;
    pOadInfoTmp = pOadInfo;
    uint8 *srcDataTmp = (uint8 *)dsdata->srcBuf;
    uint8 *dstBufTmp = (uint8 *)dsdata->dstBuf;

    /*入参检查*/
    if ((pOadInfoTmp == NULL) || (dsdata->srcBuf == NULL) || (dsdata->dstBuf == NULL) || (dsdata->sOffset == NULL))
    {
        ACUDP_FMT_TRACE("data_to_buf_class4() 入参检查 error, ERR_PTR\n");
        return ERR_PTR;
    }

    switch (pOadInfoTmp->pTab->eData)
    {
    case E_OOP_INT4V_T:                      
    {
        OOP_INT4V_T INT4V_T;
        if (dsdata->srcLen != sizeof(OOP_INT4V_T))
        {
            nRet = ERR_OPTION;
            break;
        }
        memcpy(&INT4V_T, srcDataTmp, dsdata->srcLen);
        offlen = set_len_offset(INT4V_T.nNum, lenBuf);

        dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
        offset ++;
        memcpy(&dstBufTmp[offset], lenBuf, offlen); 
        offset += offlen;

		ACUDP_FMT_TRACE("INT4V_T.nNum is %d\n",INT4V_T.nNum);
        for (i = 0; i < INT4V_T.nNum; i++)
        {
            if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dsdata->dstLen, &offset, &INT4V_T.nValue[i], sizeof(int32)) < 0)
		    {
		    	ACUDP_FMT_TRACE("offset is %d\n",offset);
		        nRet = ERR_NORMAL;
		        return nRet;
		    }
        }
        nRet = ERR_OK;
     
    }
	break;
    case E_OOP_LONG4V_T:                      
    {
        OOP_LONG4V_T LONG4V_T;
        if (dsdata->srcLen != sizeof(OOP_LONG4V_T))
        {
            nRet = ERR_OPTION;
            break;
        }
        memcpy(&LONG4V_T, srcDataTmp, dsdata->srcLen);
        offlen = set_len_offset(LONG4V_T.nNum, lenBuf);

        dstBufTmp[offset] = pOadInfoTmp->pTab->dataType;
        offset ++;
        memcpy(&dstBufTmp[offset], lenBuf, offlen); 
        offset += offlen;

        for (i = 0; i < LONG4V_T.nNum; i++)
        {
            if(basic_data_to_buf(E_LONG, dstBufTmp, dsdata->dstLen, &offset, &LONG4V_T.nValue[i], sizeof(int16)) < 0)
		    {
		        nRet = ERR_NORMAL;
		        return nRet;
		    }

        }
        nRet = ERR_OK;
       
    }
	    break;
    case E_LONG:                      
    {
        return data_to_buf_class4_base_data(pOadInfo, dsdata);
    }
	    break;
    default:
        nRet = ERR_OPTION;
        break;
    }

    *dsdata->dOffset = offset;

    return nRet;
}

OOP_DAR_E buf_to_data_class4(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}


