/*
*********************************************************************
* @file    class1.c
* @brief�� class1����
* @author��
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
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
* @brief��     698��ʽ����ת���������ĵ�����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}


/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
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
        //�ṹ����
        dstBuf[dOffset++] = DT_ARRAY;

        //�ṹ��Ա��
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

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
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
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
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
        //�ṹ����
        dstBuf[dOffset++] = DT_ARRAY;

        //�ṹ��Ա��
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
    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
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
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
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
        //�ṹ����
        dstBuf[dOffset++] = DT_ARRAY;

        //�ṹ��Ա��
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
    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
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
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
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
        //�ṹ����
        dstBuf[dOffset++] = DT_ARRAY;

        //�ṹ��Ա��
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

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
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
* @brief��     ���������ĵ�����ת698��ʽ����(class1)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class1(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->eData)
    {
        case E_OOP_ENERGY_T:   //������(��������)
        {
           return data_to_buf_class1_energy(pOadInfo,  convert);
        }break;

        case E_OOP_ENERGYS_T:   //������(������)
        {
           return data_to_buf_class1_energys(pOadInfo,  convert);
        }break;

        case E_OOP_HENERGY_T:   //�߾��ȵ�����(��������)
        {
           return data_to_buf_class1_henergy(pOadInfo,  convert);
        }break;

        case E_OOP_HENERGYS_T:   //�߾��ȵ�����(������)
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
* @name��       class1_invoke_get
* @brief��      class1��GET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
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

    //��������
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

            //ת���ɱ�������
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
    
    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class1(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class1 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��       class1_invoke_get
* @brief��      class1��GET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
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

    //��������
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

            //ת���ɱ�������
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
    
    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class1(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class1 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��       class1_invoke_get
* @brief��      class1��GET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
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

    //��������
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

            //ת���ɱ�������
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
    
    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class1(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class1 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}

/**
*********************************************************************
* @name��       class1_invoke_get
* @brief��      class1��GET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
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

    //��������
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

                //�������ĳ�����NULL
                return oop_null_array_res(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
            }

            PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nNum = %d\n", data.nNum);
            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("class1_invoke_get_class1_energy : data.nValue[%d] = %d\n", i, data.nValue[i]);
            }

            //ת���ɱ�������
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
    
    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class1(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class1 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* ��д���Ŀ�ͷ����OAD�����ݽ��0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}


#if DESC("class1����ӿں���",1)

/**
*********************************************************************
* @name��       class1_invoke_set
* @brief��      class1��SET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_set(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}

/**
*********************************************************************
* @name��       class1_invoke_get
* @brief��      class1��GET����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->eData)
    {
        case E_OOP_ENERGY_T:   //������(��������)
        {
           return class1_invoke_get_class1_energy(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case E_OOP_ENERGYS_T:   //������(������)
        {
           return class1_invoke_get_class1_energys(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case E_OOP_HENERGY_T:   //�߾��ȵ�����(��������)
        {
           return class1_invoke_get_class1_henergy(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case E_OOP_HENERGYS_T:   //�߾��ȵ�����(������)
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
* @name��       class1_invoke_act
* @brief��      class1��ACTION����
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ��OOP_DAR_E
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E class1_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
