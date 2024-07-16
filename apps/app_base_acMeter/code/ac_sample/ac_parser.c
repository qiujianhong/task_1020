/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/

#include "ac_sample698.h"
#include "ac_sample.h"
#include "ac_update.h"
#include "ac_lib.h"
#include "ac_main.h"
#include "ac_commdrv.h"
#include "ac_mqtt.h"
#include "ac_save.h"
#include "commClass.h"
#include "ac_report.h"
#include "ac_parser.h"
#include "ac_redef.h"
#include "ac_oadlist.h"

extern uint16 gAcEventNum;  //�ն˱����¼�����
extern uint8 data_type_length_get(enum tag_OOP_DATATYPE dataType);
extern OOP_OCTETVAR16_T    tmnAddr;                     //�ն�ͨ�ŵ�ַ

#if DESC("ͨ�ýӿ�", 1)
/**********************************************************************
* @name��      get_info_preproc
* @brief��     �ѱ�
* @param[in] ��oad       -  oad
               ppDataMap - ��ָ��
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int get_info_preproc(OOP_OAD_U oad, AC_PRE_PROC_T **ppDataMap)
{
    AC_PRE_PROC_T *pDataMap = NULL;
    uint32    num = 0;
    uint32    j = 0;

    pDataMap = (AC_PRE_PROC_T *)&gPreprocList[0];
    num = gPreprocListNum;
    for (j = 0; j < num; j ++)
    {
        if (pDataMap->oad.value == oad.value)
        {
            break;
        }
        pDataMap ++;
    }

    if (j == num)
    {
        return -1;
    }

    *ppDataMap = pDataMap;

    return 0;
}

/**********************************************************************
* @name��      invoke_get_with_data
* @brief��     ����apdu
* @param[in] ��ptFrame - 698��Ϣ
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int invoke_get_with_data(uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    int ret = -1;
    AC_PRE_PROC_T *pDataMap = NULL;
    OOP_OAD_U serviceOAD = {0};

    memcpy_r(&serviceOAD, &pInMsg[0], sizeof(OOP_OAD_U));

    if (get_info_preproc(serviceOAD, &pDataMap) <0)
    {
        ACSAMP_FMT_DEBUG("find no pOopParserFun. serviceOAD(0x%08x)\n", serviceOAD.value);
        return -1;
    }

    if (pDataMap->pOopParserFun != NULL)
    {
        ret = pDataMap->pOopParserFun(serviceOAD, &pInMsg[4], inLen - 4, pDataMap->destData, pDataMap->destDataBak);
        if (ret < 0)
        {
            ACSAMP_BUF_DEBUG(&pInMsg[4], inLen - 4, "pOopParserFun err. serviceOAD[0x%08x]. ret[%d]", 
                serviceOAD.value, ret);
            
            return -2;
        }
    }

    return 0;
}

/**********************************************************************
* @name��      get_normal_with_data
* @brief��     ����apdu
* @param[in] ��ptFrame - 698��Ϣ
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int get_normal_with_data(FRAME_HEAD_T *ptFrame, void *destData, void *destDataBak)
{
    int32  ret = ERR_OK;
    uint16 dataLen = 0;
    int16  offset = 0;
    OOP_OAD_U oad = {0};

    dataLen = 5; //ƫ��OAD+DATA

    memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_OFFSET + offset], sizeof(oad.value));

    //�ж��Ƿ���DATA
    if (ptFrame->pApdu[OOP_DATA_OFFSET+offset+dataLen-1] == 0)
    {
        ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen + 1, "find info from apdu oad[0x%08x] err", 
            oad.value);
        //DAR + ������
        offset += dataLen + 1;
    }
    else
    {
        if (!apdu_next_position_get(&ptFrame->pApdu[OOP_DATA_OFFSET+offset], &dataLen, ptFrame->apduLen, DT_NULL, FALSE))
        {
            return -1;
        }
        memcpy_r(&oad.value, &ptFrame->pApdu[OOP_DATA_OFFSET+offset], sizeof(oad.value));
        ACSAMP_BUF_TRACE(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen, "find info from apdu oad[0x%08x]", 
            oad.value);

        ret = invoke_get_with_data(&ptFrame->pApdu[OOP_DATA_OFFSET + offset], dataLen, destData, destDataBak);
        if (ret != ERR_OK)
        {
            return -2;
        }

        offset += dataLen; //APDU����ƫ��
    }

    return 0;
}

/**********************************************************************
* @name��      long_change_to_BCD
* @brief��     ��longת��Ϊnλ��bcd��
* @param[in] ��dec      -  Ŀ������
               n        -  ��λBCD��
               src      -  Դ����
* @param[out]��
* @return    ��
* @Date      ��2019-12-6
**********************************************************************/
void long_change_to_BCD(uint8 *dec,uint8 n,uint32 src)
{
   	uint8 tmp=0,i;

   	if((src == 0xffffffff)&&( n == 6) )
	{
		dec[0]=APP_NONECHAR;
		dec[1]=APP_NONECHAR;
		dec[2]=APP_NONECHAR;
		return;
	}
	if( (n == 4) && (src == 0xffff))
	{
		dec[0]=APP_NONECHAR;
		dec[1]=APP_NONECHAR;
		return;
	}
	if( ( src == 0xffffffff)&&( n == 8) )
	{
		dec[0]=APP_NONECHAR;
		dec[1]=APP_NONECHAR;
		dec[2]=APP_NONECHAR;
		dec[3]=APP_NONECHAR;
		return;
	}
	if(n<=0) return;
	if(n>20) return;
	for(i=1;i<=n;i++)
	{
		tmp>>=4;
		tmp+=((uint32)src%10)<<4;
		src/=10;
		if((i%2)!=0) continue;
		*dec++=tmp;
		tmp=0;
	}
	if((n%2)!=0) *dec=(tmp>>4);
}

/**********************************************************************
* @name��      get_info_map
* @brief��     �ѱ�
* @param[in] ��oad       -  oad
               ppDataMap - ��ָ��
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int get_info_map(OOP_OAD_U oad, OAD_INFO_MAP_T **ppDataMap)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    num = 0;
    uint32    i = 0;
    uint32    j = 0;

    for (i = 0; i < gOadInfoListNum; i ++)
    {
        pDataMap = gOadInfoList[i].pOadInfoMap;
        num = *gOadInfoList[i].pOadInfoNum;
        for (j = 0; j < num; j ++)
        {
            if (pDataMap->oad.value == oad.value)
            {
                break;
            }
            pDataMap ++;
        }
        if (j < num)
        {
            break;
        }
    }

    if ((i == gOadInfoListNum) && (j == num))
    {
        return -1;
    }

    *ppDataMap = pDataMap;

    return 0;
}
#endif

#if DESC("���ݽ����ӿ�", 1)

/**********************************************************************
* @name��      ac_Energy_parser
* @brief��     �������ݽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_Energy_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = -1;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    BYTE4_INT32_TYPE *pValue = (BYTE4_INT32_TYPE *)destData;
    BYTE8_INT64_TYPE *pValue64 = (BYTE8_INT64_TYPE *)destData;
    BYTE4_INT32_TYPE* pValue32 = (BYTE4_INT32_TYPE*)destDataBak;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }
    
    switch (pDataMap->structType)
    {
    case E_OOP_ENERGYS_T:
    case E_OOP_ENERGY_T:
        //ƫ�Ƶ�DATA
        offset ++;
        if (pInMsg[offset ++] != DT_ARRAY)
        {
            ret = -2;
            break;
        }
        nNum = pInMsg[offset ++];
        if (nNum != (MAX_TARIFF_NUM + 1))
        {
            ret = -3;
            break;
        }
        for (i = 0; i < nNum; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)pValue, tmpBuff, 4);
                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                offset += dataLen;
                pValue ++;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }

        ret = 0;
        break;

    case E_OOP_ENERGYS_ONE_T:
    case E_OOP_ENERGY_ONE_T:
        //ƫ�Ƶ�DATA
        offset ++;
        if (pInMsg[offset ++] != DT_ARRAY)
        {
            ret = -2;
            break;
        }
        nNum = pInMsg[offset ++];
        if (nNum != (MAX_TARIFF_NUM + 1))
        {
            ret = -3;
            break;
        }
        for (i = 0; i < 1; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)pValue, tmpBuff, 4);
                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                offset += dataLen;
                pValue ++;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }

        ret = 0;
        break;

    case E_OOP_HENERGYS_T:
    case E_OOP_HENERGY_T:
        //ƫ�Ƶ�DATA
        offset ++;
        if (pInMsg[offset ++] != DT_ARRAY)
        {
            ret = -2;
            break;
        }
        nNum = pInMsg[offset ++];
        if (nNum != (MAX_TARIFF_NUM + 1))
        {
            ret = -3;
            break;
        }
        for (i = 0; i < nNum; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)pValue64, tmpBuff, 8);

                //ͬʱ���߾��ȵ���������ͨ������
                if (pValue32 != NULL)
                {
                    pValue32->ULinteger = pValue64->ULinteger / 100;
                    pValue32++;
                }

                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%lld]", oad.value, *pValue64);
                offset += dataLen;
                pValue64 ++;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }

        ret = 0;
        break;

    case E_OOP_HENERGYS_ONE_T:
    case E_OOP_HENERGY_ONE_T:
        //ƫ�Ƶ�DATA
        offset ++;
        if (pInMsg[offset ++] != DT_ARRAY)
        {
            ret = -2;
            break;
        }
        nNum = pInMsg[offset ++];
        if (nNum != (MAX_TARIFF_NUM + 1))
        {
            ret = -3;
            break;
        }
        for (i = 0; i < 1; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)pValue64, tmpBuff, 8);

                //ͬʱ���߾��ȵ���������ͨ������
                if (pValue32 != NULL)
                {
                    pValue32->ULinteger = pValue64->ULinteger / 100;
                    pValue32++;
                }

                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%lld]", oad.value, *pValue64);
                offset += dataLen;
                pValue64 ++;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }

        ret = 0;
        break;

    default:
        break;
    }

    return ret;
}

/**********************************************************************
* @name��      ac_Demand_parser
* @brief��     �������ݽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_Demand_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = -1;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int32     tmpValue = 0;
    uint8     signFlag = 0; 
    uint16    year = 0;
    DEM_NUM_TYPE *pDemandValue = (DEM_NUM_TYPE *)destData;
    TERM_DEM_TIME_TYPE *pDemandTime = (TERM_DEM_TIME_TYPE *)destDataBak;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }
    
    switch (pDataMap->structType)
    {
    case E_OOP_DEMAND_T:
    case E_OOP_DEMANDS_T:
        //ƫ�Ƶ�DATA
        offset ++;
        if (pInMsg[offset ++] != DT_ARRAY)
        {
            ret = -2;
            break;
        }
        nNum = pInMsg[offset ++];
        if (nNum != (MAX_TARIFF_NUM + 1))
        {
            ret = -3;
            break;
        }
        for (i = 0; i < nNum; i++)
        {
            //����ֵ
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                if(dataLen == 1)    //dataΪNULL
                {
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                    offset += dataLen;
                    pDemandValue ++;
                    pDemandTime ++;
                    continue;
                }
                //�����Ļ���ֻ��3���ֽڣ�ת����Ҫɾ��һ���ֽڵ���Ч����
                memcpy_r((uint8 *)&tmpValue, &tmpBuff[0], 4);
                if (tmpValue < 0)
                {
                    signFlag = 1;
                    tmpValue = abs(tmpValue);
                }
                long_change_to_BCD((uint8 *)pDemandValue, 6, tmpValue);
                if (signFlag == 1)
                {
                    pDemandValue->DemNum[2] |= 0x80;
                }
                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, tmpValue);
                offset += dataLen;
                pDemandValue ++;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }

            //ʱ��
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                if(dataLen == 1)    //dataΪNULL
                {
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). index[%d] time is NULL", oad.value, i+1);
                    offset += dataLen;
                    pDemandTime ++;
                    continue;
                }
                pDemandTime->DemTime[0] = 0;
                memcpy_r(&year, &tmpBuff[0], sizeof(year));
                pDemandTime->DemTime[1] = Hex2Bcd(tmpBuff[5]);
                pDemandTime->DemTime[2] = Hex2Bcd(tmpBuff[4]);
                pDemandTime->DemTime[3] = Hex2Bcd(tmpBuff[3]);
                pDemandTime->DemTime[4] = Hex2Bcd(tmpBuff[2]);
                if (year >= 2000)
                {
                    pDemandTime->DemTime[5] = Hex2Bcd(year - 2000);
                }
                else
                {
                    pDemandTime->DemTime[5] = 0;
                    ACSAMP_FMT_TRACE("err:oad(0x%08x). year[%d]\n", oad.value, year);
                }
                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[0x%012x]", oad.value, *pDemandTime);
                offset += dataLen;
                pDemandTime ++;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -5;
            }

            ret = 0;
        }
        break;

    case E_OOP_DEMAND_ONE_T:
    case E_OOP_DEMANDS_ONE_T:
        {
            //ƫ�Ƶ�DATA
            offset ++;
            if (pInMsg[offset ++] != DT_ARRAY)
            {
                ret = -6;
                break;
            }
            nNum = pInMsg[offset ++];
            if (nNum != 1)
            {
                ret = -7;
                break;
            }
            for (i = 0; i < 1; i++)
            {
                //����ֵ
                dataLen = 0;
                if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
                {
                    if (dataLen == 1)    //dataΪNULL
                    {
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                        offset += dataLen;
                        pDemandValue ++;
                        pDemandTime ++;
                        continue;
                    }

                   //�����Ļ���ֻ��3���ֽڣ�ת����Ҫɾ��һ���ֽڵ���Ч����
                    memcpy_r((uint8 *)&tmpValue, &tmpBuff[0], 4);
                    if (tmpValue < 0)
                    {
                        signFlag = 1;
                        tmpValue = abs(tmpValue);
                    }
                    long_change_to_BCD((uint8 *)pDemandValue, 6, tmpValue);
                    if (signFlag == 1)
                    {
                        pDemandValue->DemNum[2] |= 0x80;
                    }
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, tmpValue);
                    offset += dataLen;
                    pDemandValue ++;
                }
                else
                {
                    ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                    return -8;
                }

                //ʱ��
                dataLen = 0;
                if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
                {
                    if (dataLen == 1)    //dataΪNULL
                    {
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). index[%d] time is NULL", oad.value, i+1);
                        offset += dataLen;
                        pDemandTime ++;
                        continue;
                    }

                    pDemandTime->DemTime[0] = 0;
                    memcpy_r(&year, &tmpBuff[0], sizeof(year));
                    pDemandTime->DemTime[1] = Hex2Bcd(tmpBuff[5]);
                    pDemandTime->DemTime[2] = Hex2Bcd(tmpBuff[4]);
                    pDemandTime->DemTime[3] = Hex2Bcd(tmpBuff[3]);
                    pDemandTime->DemTime[4] = Hex2Bcd(tmpBuff[2]);
                    if (year >= 2000)
                    {
                        pDemandTime->DemTime[5] = Hex2Bcd(year - 2000);
                    }
                    else
                    {
                        pDemandTime->DemTime[5] = 0;
                        ACSAMP_FMT_TRACE("err:oad(0x%08x). year[%d]\n", oad.value, year);
                    }
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[0x%012x]", oad.value, *pDemandTime);
                    offset += dataLen;
                    pDemandTime ++;
                }
                else
                {
                    ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                    return -9;
                }
            }

            ret = 0;
        }
        break;
        
    default:
        break;
    }

    return ret;
}

/**********************************************************************
* @name��      ac_Demand_parser
* @brief��     �������ݽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_Hormonic_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = -1;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    uint16 *pHarmonic = (uint16 *)destData;
    uint32 *pHarmonic_I = (uint32 *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }
    
    switch (pDataMap->structType)
    {
    case E_OOP_HARMONIC_T:
    case E_OOP_HARMONIC_U_VALUE_T:
        //ƫ�Ƶ�DATA
        offset ++;
        if (pInMsg[offset ++] != DT_ARRAY)
        {
            ret = -2;
            break;
        }
        nNum = pInMsg[offset ++];
        if (nNum != MAX_HARMONIC_NUM)
        {
            ret = -3;
            break;
        }
        for (i = 0; i < nNum; i++)
        {
            //г��������
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
               //�����Ļ���ֻ��3���ֽڣ�ת����Ҫɾ��һ���ֽڵ���Ч����
                memcpy_r((uint8 *)pHarmonic, &tmpBuff[0], 2);
                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pHarmonic);
                offset += dataLen;
                pHarmonic ++;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }

            ret = 0;
        }
        break;

    case E_OOP_HARMONIC_I_VALUE_T:
        //ƫ�Ƶ�DATA
        offset ++;
        if (pInMsg[offset ++] != DT_ARRAY)
        {
            ret = -5;
            break;
        }
        nNum = pInMsg[offset ++];
        if (nNum != MAX_HARMONIC_NUM)
        {
            ret = -6;
            break;
        }
        for (i = 0; i < nNum; i++)
        {
            //����г��������
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
               //�����Ļ���ֻ��3���ֽڣ�ת����Ҫɾ��һ���ֽڵ���Ч����
                memcpy_r((uint8 *)pHarmonic_I, &tmpBuff[0], 4);
                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pHarmonic_I);
                offset += dataLen;
                pHarmonic_I ++;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -7;
            }

            ret = 0;
        }
        break;
        
    default:
        break;
    }

    return ret;
}

/**********************************************************************
* @name��       ac_Volt_parser
* @brief��      ��ѹ����
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_Volt_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    uint16    *pValue = (uint16 *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    nNum = pInMsg[offset ++];
    if (nNum != 3)
    {
        return -3;
    }
    for (i = 0; i < nNum; i++)
    {
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            memcpy_r((uint8 *)pValue, tmpBuff, 2);
            ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
            pValue ++;
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -4;
        }
    }

    return 0;
}

/**********************************************************************
* @name��       ac_VoltHD_parser
* @brief��      ��ѹ����
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_VoltHD_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    uint16    *pValue = (uint16 *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    nNum = pInMsg[offset ++];
    if (nNum != 3)
    {
        return -3;
    }
    for (i = 0; i < nNum; i++)
    {
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            memcpy_r((uint8 *)pValue, tmpBuff, 4);
            ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
            pValue ++;
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -4;
        }
    }

    return 0;
}

/**********************************************************************
* @name��       ac_Curr_parser
* @brief��      ��������
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_Curr_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int32     srcValue = 0;
    uint32    *pValue = (uint32 *)destData;
    uint8     signFlag = 0;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    nNum = pInMsg[offset ++];
    if (nNum != 3)
    {
        return -3;
    }
    for (i = 0; i < nNum; i++)
    {
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            memcpy_r((uint8 *)&srcValue, tmpBuff, 4);
            if (srcValue < 0)
            {
                signFlag = 1;
            }
            else
            {
                signFlag = 0;
            }

            //���Ŵ����˽ӿ�ֻ���ڵ���
            if (i == 0)
            {
                gPriMeasurePara.CurrentSignType.PhaAAppCurrentFlag = signFlag;
            }
            else if (i == 1)
            {
                gPriMeasurePara.CurrentSignType.PhaBAppCurrentFlag = signFlag;
            }
            else if (i == 2)
            {
                gPriMeasurePara.CurrentSignType.PhaCAppCurrentFlag = signFlag;
            }
            
            (*pValue) = abs(srcValue);
            ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value1[%d] signFlag[%d]", 
                oad.value, *pValue, signFlag);
            pValue ++;
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -4;
        }
    }

    return 0;
}

/**********************************************************************
* @brief��     ���ߵ�������
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_zero_line_Curr_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int32     srcValue = 0;
    uint32    *pValue = (uint32 *)destData;
    uint8     signFlag = 0;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset] != DT_DOUBLE_LONG)
    {
        ACSAMP_FMT_DEBUG("data type err oad(0x%08x). type[0x%x]\n", oad.value, pInMsg[offset]);
        return -2;
    }

    if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
    {
        memcpy_r((uint8 *)&srcValue, tmpBuff, 4);
        if (srcValue < 0)
        {
            signFlag = 1;
        }
        else
        {
            signFlag = 0;
        }

        //���Ŵ����˽ӿ�ֻ���ڵ���
        gPriMeasurePara.CurrentSignType.ZeroLineCurrentFlag = signFlag;
        
        (*pValue) = abs(srcValue);
        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
        offset += dataLen;
    }
    else
    {
        ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
        return -4;
    }

    return 0;
}

/**********************************************************************
* @brief��      ���ߵ�������
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_zero_Curr_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int32     srcValue = 0;
    uint32    *pValue = (uint32 *)destData;
    uint8     signFlag = 0;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset] != DT_DOUBLE_LONG)
    {
        ACSAMP_FMT_DEBUG("data type err oad(0x%08x). type[0x%x]\n", oad.value, pInMsg[offset]);
        return -2;
    }

    if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
    {
        memcpy_r((uint8 *)&srcValue, tmpBuff, 4);
        if (srcValue < 0)
        {
            signFlag = 1;
        }
        else
        {
            signFlag = 0;
        }

        //���Ŵ����˽ӿ�ֻ���ڵ���
        gPriMeasurePara.CurrentSignType.ZeroCurrentFlag = signFlag;
        
        (*pValue) = abs(srcValue);
        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
        offset += dataLen;
    }
    else
    {
        ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
        return -4;
    }

    return 0;
}

/**********************************************************************
* @name��       ac_Power_parser
* @brief��      ���ʽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_ActPower_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int32     tmpValue = 0;
    int32     *pValue = (int32 *)destData;
    
    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    switch (pDataMap->structType)
    {
    case E_OOP_INT4V_T:
        //ƫ�Ƶ�DATA
        offset ++;

        if (pInMsg[offset ++] != DT_ARRAY)
        {
            return -2;
        }
        nNum = pInMsg[offset ++];
        if (nNum != 4)
        {
            return -3;
        }
        for (i = 0; i < nNum; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)&tmpValue, tmpBuff, 4);
                //����Ҫ���⴦����������
                if (i == 0)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.ActPowFlag = 0;
                        *pValue = tmpValue;
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                        pValue ++;
                        *pValue = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.ActPowFlag = 1;
                        *pValue = 0;
                        pValue ++;
                        *pValue = abs(tmpValue);
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                    }
                }
                else if(i == 1)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaAActPowFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaAActPowFlag = 1;
                    }
                    *pValue = abs(tmpValue);
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }
                else if(i == 2)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaBActPowFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaBActPowFlag = 1;
                    }
                    *pValue = abs(tmpValue);
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }
                else if(i == 3)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaCActPowFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaCActPowFlag = 1;
                    }
                    *pValue = abs(tmpValue);
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }

                pValue ++;
                offset += dataLen;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }
        break;
    
    default:
        break;
    }

    return 0;    
}

/**********************************************************************
* @name��       ac_Power_parser
* @brief��      ���ʽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_ReaPower_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int32     tmpValue = 0;
    int32     *pValue = (int32 *)destData;
    
    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    switch (pDataMap->structType)
    {
    case E_OOP_INT4V_T:
        //ƫ�Ƶ�DATA
        offset ++;

        if (pInMsg[offset ++] != DT_ARRAY)
        {
            return -2;
        }
        nNum = pInMsg[offset ++];
        if (nNum != 4)
        {
            return -3;
        }
        for (i = 0; i < nNum; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)&tmpValue, tmpBuff, 4);
                //����Ҫ���⴦����������
                if (i == 0)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.ReaPowFlag = 0;
                        *pValue = tmpValue;
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                        pValue ++;
                        *pValue = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.ReaPowFlag = 1;
                        *pValue = 0;
                        pValue ++;
                        *pValue = abs(tmpValue);
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                    }
                }
                else if(i == 1)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaAReaPowFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaAReaPowFlag = 1;
                    }
                    *pValue = abs(tmpValue);
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }
                else if(i == 2)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaBReaPowFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaBReaPowFlag = 1;
                    }
                    *pValue = abs(tmpValue);
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }
                else if(i == 3)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaCReaPowFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaCReaPowFlag = 1;
                    }
                    *pValue = abs(tmpValue);
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }

                pValue ++;
                offset += dataLen;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }
        break;
    
    default:
        break;
    }

    return 0;    
}

/**********************************************************************
* @name��       ac_Power_parser
* @brief��      ���ʽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_AppPower_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int32     tmpValue = 0;
    int32     *pValue = (int32 *)destData;
    
    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    switch (pDataMap->structType)
    {
    case E_OOP_INT4V_T:
        //ƫ�Ƶ�DATA
        offset ++;

        if (pInMsg[offset ++] != DT_ARRAY)
        {
            return -2;
        }
        nNum = pInMsg[offset ++];
        if (nNum != 4)
        {
            return -3;
        }
        for (i = 0; i < nNum; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)&tmpValue, tmpBuff, 4);
                //����Ҫ���⴦����������
                if (i == 0)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.AppPowFlag = 0;
                        *pValue = tmpValue;
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                        pValue ++;
                        *pValue = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.AppPowFlag = 1;
                        *pValue = 0;
                        pValue ++;
                        *pValue = abs(tmpValue);
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                    }
                }
                else if(i == 1)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaAAppPowFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaAAppPowFlag = 1;
                    }
                    *pValue = abs(tmpValue);
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }
                else if(i == 2)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaBAppPowFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaBAppPowFlag = 1;
                    }
                    *pValue = abs(tmpValue);
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }
                else if(i == 3)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaCAppPowFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.CurPowDir.BitSect.PhaCAppPowFlag = 1;
                    }
                    *pValue = abs(tmpValue);
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }

                pValue ++;
                offset += dataLen;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }
        break;
    
    default:
        break;
    }

    return 0;    
}

/**********************************************************************
* @name��       ac_Power_parser
* @brief��      ���ʽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_PowerFactor_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int16     tmpValue = 0;
    int16     *pValue16 = (int16 *)destData;
    
    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    switch (pDataMap->structType)
    {
    case E_OOP_LONG4V_T:
        //ƫ�Ƶ�DATA
        offset ++;

        if (pInMsg[offset ++] != DT_ARRAY)
        {
            return -2;
        }
        nNum = pInMsg[offset ++];
        if (nNum != 4)
        {
            return -3;
        }
        for (i = 0; i < nNum; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)&tmpValue, tmpBuff, 2);
                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue16);
                if (i == 0)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.PowerFactorType.AppPfFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.PowerFactorType.AppPfFlag = 1;
                    }
                }
                else if (i == 1)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.PowerFactorType.PhaAAppPfFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.PowerFactorType.PhaAAppPfFlag = 1;
                    }
                }
                else if (i == 2)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.PowerFactorType.PhaBAppPfFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.PowerFactorType.PhaBAppPfFlag = 1;
                    }
                }
                else if (i == 3)
                {
                    if (tmpValue > 0)
                    {
                        gPriMeasurePara.PowerFactorType.PhaCAppPfFlag = 0;
                    }
                    else
                    {
                        gPriMeasurePara.PowerFactorType.PhaCAppPfFlag = 1;
                    }
                }
                *pValue16 = abs(tmpValue);
                pValue16 ++;
                offset += dataLen;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }
        break;
    
    default:
        break;
    }

    return 0;
}


/**********************************************************************
* @name��       ac_Power_parser
* @brief��      ���ʽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_Power_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int32     tmpValue = 0;
    int32     *pValue = (int32 *)destData;
    int16     *pValue16 = (int16 *)destData;
    
    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    switch (pDataMap->structType)
    {
    case E_OOP_INT4V_T:
        //ƫ�Ƶ�DATA
        offset ++;

        if (pInMsg[offset ++] != DT_ARRAY)
        {
            return -2;
        }
        nNum = pInMsg[offset ++];
        if (nNum != 4)
        {
            return -3;
        }
        for (i = 0; i < nNum; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)&tmpValue, tmpBuff, 4);
                //����Ҫ���⴦����������
                if (i == 0)
                {
                    if (tmpValue > 0)
                    {
                        *pValue = tmpValue;
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                        pValue ++;
                        *pValue = 0;
                    }
                    else
                    {
                        *pValue = 0;
                        pValue ++;
                        *pValue = abs(tmpValue);
                        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                    }
                }
                else
                {
                    *pValue = tmpValue;
                    ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
                }
                pValue ++;
                offset += dataLen;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }
        break;

    case E_OOP_LONG4V_T:
        //ƫ�Ƶ�DATA
        offset ++;

        if (pInMsg[offset ++] != DT_ARRAY)
        {
            return -2;
        }
        nNum = pInMsg[offset ++];
        if (nNum != 4)
        {
            return -3;
        }
        for (i = 0; i < nNum; i++)
        {
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                memcpy_r((uint8 *)pValue16, tmpBuff, 2);
                ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue16);
                pValue16 ++;
                offset += dataLen;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
        }
        break;
    
    default:
        break;
    }

    return 0;
}

/**********************************************************************
* @name��       ac_uint16_parser
* @brief��      ͨ�����ݽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_uint16_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    uint16     *pValue = (uint16 *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset] != DT_LONG_UNSIGNED)
    {
        ACSAMP_FMT_DEBUG("data type err oad(0x%08x). type[0x%x]\n", oad.value, pInMsg[offset]);
        return -2;
    }

    if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
    {
        memcpy_r((uint8 *)pValue, tmpBuff, 2);
        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
        pValue ++;
        offset += dataLen;
    }
    else
    {
        ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
        return -3;
    }

    return 0;
}


/**********************************************************************
* @name��      ac_int32_parser
* @brief��     ͨ�����ݽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_int32_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int32     *pValue = (int32 *)destData;
    uint8     *pValue_D = (uint8 *)destDataBak;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset] != DT_DOUBLE_LONG)
    {
        ACSAMP_FMT_DEBUG("data type err oad(0x%08x). type[0x%x]\n", oad.value, pInMsg[offset]);
        return -2;
    }

    if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
    {
        memcpy_r((uint8 *)pValue, tmpBuff, 4);
        if (*pValue < 0)
        {
            if (pValue_D != NULL)
            {
                *pValue_D = 1;
                pValue_D ++;
            }
        }
        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
        pValue ++;
        offset += dataLen;
    }
    else
    {
        ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
        return -3;
    }

    return 0;
}

/**********************************************************************
* @name��      ac_int16_parser
* @brief��     ͨ�����ݽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_int16_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int16     *pValue = (int16 *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset] != DT_LONG)
    {
        ACSAMP_FMT_DEBUG("data type err oad(0x%08x). type[0x%x]\n", oad.value, pInMsg[offset]);
        return -2;
    }

    if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
    {
        memcpy_r((uint8 *)pValue, tmpBuff, 2);
        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
        pValue ++;
        offset += dataLen;
    }
    else
    {
        ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
        return -3;
    }

    return 0;
}

/**********************************************************************
* @name��      ac_uint32_parser
* @brief��     ͨ�����ݽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_uint32_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    uint32     *pValue = (uint32 *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset] != DT_DOUBLE_LONG_UNSIGNED)
    {
        ACSAMP_FMT_DEBUG("data type err oad(0x%08x). type[0x%x]\n", oad.value, pInMsg[offset]);
        return -2;
    }

    if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
    {
        memcpy_r((uint8 *)pValue, tmpBuff, 4);
        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
        pValue ++;
        offset += dataLen;
    }
    else
    {
        ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
        return -3;
    }

    return 0;
}

/**********************************************************************
* @name��      ac_uint32_parser
* @brief��     Ƶ�����ݽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_HZ_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    uint32    valueTmp = 0;
    uint32    *pValue = (uint32 *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset] != DT_DOUBLE_LONG_UNSIGNED)
    {
        ACSAMP_FMT_DEBUG("data type err oad(0x%08x). type[0x%x]\n", oad.value, pInMsg[offset]);
        return -2;
    }

    if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
    {
        memcpy_r((uint8 *)&valueTmp, tmpBuff, 4);
        *pValue = valueTmp;
        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
        offset += dataLen;
    }
    else
    {
        ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
        return -3;
    }

    return 0;
}

/**********************************************************************
* @name��      ac_PhAngle_parser
* @brief��     ��Ǵ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_PhAngle_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    int16     *pValue = (int16 *)destData;
    uint16    *bitValue = (uint16 *)destDataBak;
        

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    nNum = pInMsg[offset ++];
    if (nNum != 3)
    {
        return -3;
    }
    for (i = 0; i < nNum; i++)
    {
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            memcpy_r((uint8 *)pValue, tmpBuff, 2);
            if (*pValue < 0)
            {
                if (bitValue != NULL)
                {
                    *bitValue |= (0x01 << i);
                }
            }
            else
            {
                if (bitValue != NULL)
                {
                    *bitValue &= ~(0x01 << i);
                }
            }
            *pValue = abs(*pValue);
            ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
            pValue ++;
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -4;
        }
    }

    return 0;
}



/**********************************************************************
* @name��      ac_Version_parser
* @brief��     �汾��Ϣ����
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_Version_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    char      strVer[30] = {0};
    uint8     *pValue = (uint8 *)destData;
    uint8     *pValue1 = (uint8*)destDataBak;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_STRUCTURE)
    {
        return -2;
    }
    nNum = pInMsg[offset ++];
    if (nNum != 6)
    {
        return -3;
    }
    for (i = 0; i < nNum; i++)
    {
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            memcpy(strVer, tmpBuff, dataLen - 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). value[%s]", oad.value, strVer);
            offset += dataLen;  
            memset(strVer, 0, sizeof(strVer));
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -4;
        }

        if (i == 1)
        {
            memcpy((uint8 *)pValue, tmpBuff, 4);
        }

        if (i == 2)
        {
            memcpy((uint8*)pValue1, tmpBuff, 6);
        }
        if (i == 5)
        {
            memcpy((uint8*)&gVerSBuf[0], tmpBuff, 8);
        }
    }

    return 0;
}

/**********************************************************************
* @name��      ac_TransStatus_parser
* @brief��     �汾��Ϣ����
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_TransStatus_parser(OOP_OAD_U oad, uint8* pInMsg, uint32 inLen, void* destData, void* destDataBak)
{
    OAD_INFO_MAP_T* pDataMap = NULL;
    uint32    offset = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[256] = {0};
    uint8* pValue = (uint8*)destData;
    uint32* pValue1 = (uint32*)destDataBak;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n",
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) < 0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset++;

    if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
    {
        *pValue1 = dataLen;
        memcpy(pValue, tmpBuff, dataLen);
        ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). len[%d]", oad.value, *pValue1);
        offset += dataLen;
    }
    else
    {
        ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
        return -4;
    }

    return 0;
}


/**********************************************************************
* @name��      ac_MeterSstate_parser
* @brief��     �������״̬�ִ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_MeterSstate_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = -1;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    uint16    *pValue = (uint16 *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    nNum = pInMsg[offset ++];
    if (nNum != 7)
    {
        return -7;
    }
    for (i = 0; i < nNum; i++)
    {
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            memcpy((uint8 *)pValue, tmpBuff, 2);
            membitrev((uint8 *)pValue, 16);
            ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
            pValue ++;
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -4;
        }
    }

    return 0;

}

/**********************************************************************
* @name��      ac_uint32_parser
* @brief��     Ƶ�����ݽ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_PowerType_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    OAD_INFO_MAP_T *pDataMap = NULL;
    uint32    offset = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    uint8    *pValue = (uint8 *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    if (get_info_map(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset] != DT_UNSIGNED)
    {
        ACSAMP_FMT_DEBUG("data type err oad(0x%08x). type[0x%x]\n", oad.value, pInMsg[offset]);
        return -2;
    }

    if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
    {
        memcpy((uint8 *)pValue, tmpBuff, 1);
        ACSAMP_BUF_TRACE(tmpBuff, dataLen, "oad(0x%08x). value[%d]", oad.value, *pValue);
        offset += dataLen;
    }
    else
    {
        ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
        return -3;
    }

    return 0;
}

#endif

#if DESC("Ԥ�������ݽ����ӿ�", 1)
/**********************************************************************
* @name��       ac_Set_normal_parser
* @brief��      ���ñ�����ʱ�α�
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_Set_normal_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    int    ret = -1;
    FRAME_HEAD_T tFrame = {0};
    uint8* ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;
    uint8   recvBufTmp[RECV_BUF_LEN] = { 0 };

    while ((ptr = get_68ptr_from_buf(pInMsg, inLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);

        inLen -= (offset + outLen);
        memmove(pInMsg, (ptr + outLen), inLen);

        /* �Ϸ��Լ�� */
        ret = app_check_message(recvBufTmp, outLen);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_check_message error[%d] msg recv", ret);
            continue;
        }

        /* ����֡ͷ */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        ret = app_get_frame(recvBufTmp, outLen, &tFrame, TRUE);
        if (0 != ret)
        {
            ACMSG_BUF_DEBUG(recvBufTmp, outLen, "app_get_frame error[%d] msg recv", ret);
            continue;
        }

        if (tFrame.pApdu[7] != DATA_SUCCESS)
        {
            return tFrame.pApdu[7];
        }
        else
        {
            break;
        }
    }

    if (ptr == NULL)
    {
        return ERR_ITEM;
    }

    return ERR_OK;
}

/**********************************************************************
* @name��      ac_MeterSstate_parser
* @brief��     �������״̬�ִ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_get_40160201_parser_fj(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    AC_PRE_PROC_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    DAYPERIOD data = {0};
    DAYPERIOD dataSet = {0};
    
    if (get_info_preproc(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    nNum = pInMsg[offset ++];
    if (nNum != 9)
    {
        return -7;
    }
    
    data.nNum = 9;
    
    for (i = 0; i < nNum; i++)
    {
        //ʱ
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            if(dataLen == 1)    //dataΪNULL
            {
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                offset += dataLen;
                continue;
            }
            
            memcpy_r((uint8 *)&data.period[i].hour, &tmpBuff[0], 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].hour[%d]", oad.value, i, data.period[i].hour);
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -4;
        }
        
        //��
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            if(dataLen == 1)    //dataΪNULL
            {
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                offset += dataLen;
                continue;
            }
            
            memcpy_r((uint8 *)&data.period[i].min, &tmpBuff[0], 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].min[%d]", oad.value, i, data.period[i].min);
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -5;
        }

        //����
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            if(dataLen == 1)    //dataΪNULL
            {
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                offset += dataLen;
                continue;
            }
            
            memcpy_r((uint8 *)&data.period[i].rateNO, &tmpBuff[0], 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].rateNO[%d]", oad.value, i, data.period[i].rateNO);
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -6;
        }
    }

    //�����׼���������ڱȽϣ���������
    dataSet.nNum = 9;
    dataSet.period[0].hour = 7;
    dataSet.period[0].min = 0;
    dataSet.period[0].rateNO = 4;
    
    dataSet.period[1].hour = 8;
    dataSet.period[1].min = 30;
    dataSet.period[1].rateNO = 3;

    dataSet.period[2].hour = 11;
    dataSet.period[2].min = 30;
    dataSet.period[2].rateNO = 2;

    dataSet.period[3].hour = 14;
    dataSet.period[3].min = 30;
    dataSet.period[3].rateNO = 3;

    dataSet.period[4].hour = 17;
    dataSet.period[4].min = 30;
    dataSet.period[4].rateNO = 2;

    dataSet.period[5].hour = 19;
    dataSet.period[5].min = 0;
    dataSet.period[5].rateNO = 3;

    dataSet.period[6].hour = 21;
    dataSet.period[6].min = 0;
    dataSet.period[6].rateNO = 2;

    dataSet.period[7].hour = 23;
    dataSet.period[7].min = 0;
    dataSet.period[7].rateNO = 3;

    dataSet.period[8].hour = 24;
    dataSet.period[8].min = 0;
    dataSet.period[8].rateNO = 4;

    for (i = 0; i < 9; i++)
    {
        if ((dataSet.period[i].hour != data.period[i].hour) || 
            (dataSet.period[i].min != data.period[i].min) ||
            (dataSet.period[i].rateNO != data.period[i].rateNO))
        {
            ACSAMP_FMT_DEBUG("��������ȷ [%d]��׼:%d-%d-%d ʵ��:%d-%d-%d\n", i, dataSet.period[i].hour, dataSet.period[i].min, 
                dataSet.period[i].rateNO, data.period[i].hour, data.period[i].min, data.period[i].rateNO);
            return -7;
        }
    }

    return 0;

}

/**********************************************************************
* @name��      ac_MeterSstate_parser
* @brief��     �������״̬�ִ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_get_40160200_parser_sd(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    uint32    j = 0;
    AC_PRE_PROC_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = 0;
    uint32    pNum = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    DAYPERIOD data = {0};
    DAYPERIOD dataSet = {0};
    
    if (get_info_preproc(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    pNum = pInMsg[offset ++];
    if (pNum != 8)
    {
        return -8;
    }

    for (j = 0; j < pNum; j++)
    {
        if (pInMsg[offset ++] != DT_ARRAY)
        {
            return -2;
        }
        nNum = pInMsg[offset ++];
        if (nNum != 14)
        {
            return -7;
        }
        
        data.nNum = 14;
        
        for (i = 0; i < nNum; i++)
        {
            //ʱ
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                if(dataLen == 1)    //dataΪNULL
                {
                    ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                    offset += dataLen;
                    continue;
                }
                
                memcpy_r((uint8 *)&data.period[i].hour, &tmpBuff[0], 1);
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].hour[%d]", oad.value, i, data.period[i].hour);
                offset += dataLen;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -4;
            }
            
            //��
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                if(dataLen == 1)    //dataΪNULL
                {
                    ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                    offset += dataLen;
                    continue;
                }
                
                memcpy_r((uint8 *)&data.period[i].min, &tmpBuff[0], 1);
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].min[%d]", oad.value, i, data.period[i].min);
                offset += dataLen;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -5;
            }
        
            //����
            dataLen = 0;
            if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
            {
                if(dataLen == 1)    //dataΪNULL
                {
                    ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                    offset += dataLen;
                    continue;
                }
                
                memcpy_r((uint8 *)&data.period[i].rateNO, &tmpBuff[0], 1);
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].rateNO[%d]", oad.value, i, data.period[i].rateNO);
                offset += dataLen;
            }
            else
            {
                ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
                return -6;
            }
        }
        
        //�����׼���������ڱȽ�
        dataSet.nNum = 14;
        dataSet.period[0].hour = 7;
        dataSet.period[0].min = 0;
        dataSet.period[0].rateNO = 3;
        
        dataSet.period[1].hour = 8;
        dataSet.period[1].min = 30;
        dataSet.period[1].rateNO = 2;
        
        dataSet.period[2].hour = 10;
        dataSet.period[2].min = 0;
        dataSet.period[2].rateNO = 1;
        
        dataSet.period[3].hour = 11;
        dataSet.period[3].min = 0;
        dataSet.period[3].rateNO = 3;
        
        dataSet.period[4].hour = 12;
        dataSet.period[4].min = 0;
        dataSet.period[4].rateNO = 4;
        
        dataSet.period[5].hour = 13;
        dataSet.period[5].min = 0;
        dataSet.period[5].rateNO = 3;
        
        dataSet.period[6].hour = 14;
        dataSet.period[6].min = 30;
        dataSet.period[6].rateNO = 2;
        
        dataSet.period[7].hour = 19;
        dataSet.period[7].min = 0;
        dataSet.period[7].rateNO = 1;
        
        dataSet.period[8].hour = 21;
        dataSet.period[8].min = 0;
        dataSet.period[8].rateNO = 3;

        dataSet.period[9].hour = 23;
        dataSet.period[9].min = 0;
        dataSet.period[9].rateNO = 4;

        dataSet.period[10].hour = 23;
        dataSet.period[10].min = 0;
        dataSet.period[10].rateNO = 4;

        dataSet.period[11].hour = 23;
        dataSet.period[11].min = 0;
        dataSet.period[11].rateNO = 4;

        dataSet.period[12].hour = 23;
        dataSet.period[12].min = 0;
        dataSet.period[12].rateNO = 4;

        dataSet.period[13].hour = 23;
        dataSet.period[13].min = 0;
        dataSet.period[13].rateNO = 4;
        
        for (i = 0; i < 14; i++)
        {
            if ((dataSet.period[i].hour != data.period[i].hour) || 
                (dataSet.period[i].min != data.period[i].min) ||
                (dataSet.period[i].rateNO != data.period[i].rateNO))
            {
                ACSAMP_FMT_DEBUG("��������ȷ [%d]��׼:%d-%d-%d ʵ��:%d-%d-%d\n", i, dataSet.period[i].hour, dataSet.period[i].min, 
                    dataSet.period[i].rateNO, data.period[i].hour, data.period[i].min, data.period[i].rateNO);
                return -7;
            }
        }
    }

    return 0;

}

/**********************************************************************
* @name��      ac_MeterSstate_parser
* @brief��     �������״̬�ִ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_get_40140200_parser_sd(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    AC_PRE_PROC_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    OOP_TIMEZONE_T data = {0};
    OOP_TIMEZONE_T dataSet = {0};
    
    if (get_info_preproc(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    nNum = pInMsg[offset ++];
    if (nNum != 14)
    {
        return -7;
    }
    
    data.nNum = 14;
    
    for (i = 0; i < nNum; i++)
    {
        //��
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            if(dataLen == 1)    //dataΪNULL
            {
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                offset += dataLen;
                continue;
            }
            
            memcpy_r((uint8 *)&data.period[i].mon, &tmpBuff[0], 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].mon[%d]", oad.value, i, data.period[i].mon);
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x) parser fail", oad.value);
            return -4;
        }
        
        //��
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            if(dataLen == 1)    //dataΪNULL
            {
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                offset += dataLen;
                continue;
            }
            
            memcpy_r((uint8 *)&data.period[i].day, &tmpBuff[0], 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].day[%d]", oad.value, i, data.period[i].day);
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x) parser fail", oad.value);
            return -5;
        }

        //ʱ�α��
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            if(dataLen == 1)    //dataΪNULL
            {
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                offset += dataLen;
                continue;
            }
            
            memcpy_r((uint8 *)&data.period[i].meterNum, &tmpBuff[0], 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].meterNum[%d]", oad.value, i, data.period[i].meterNum);
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x) parser fail", oad.value);
            return -6;
        }
    }

    //�����׼���������ڱȽϣ���������
    dataSet.nNum = 14;
    dataSet.period[0].mon = 1;
    dataSet.period[0].day = 1;
    dataSet.period[0].meterNum = 1;
    
    dataSet.period[1].mon = 1;
    dataSet.period[1].day = 1;
    dataSet.period[1].meterNum = 1;

    dataSet.period[2].mon = 1;
    dataSet.period[2].day = 1;
    dataSet.period[2].meterNum = 1;

    dataSet.period[3].mon = 1;
    dataSet.period[3].day = 1;
    dataSet.period[3].meterNum = 1;

    dataSet.period[4].mon = 1;
    dataSet.period[4].day = 1;
    dataSet.period[4].meterNum = 1;

    dataSet.period[5].mon = 1;
    dataSet.period[5].day = 1;
    dataSet.period[5].meterNum = 1;

    dataSet.period[6].mon = 1;
    dataSet.period[6].day = 1;
    dataSet.period[6].meterNum = 1;

    dataSet.period[7].mon = 1;
    dataSet.period[7].day = 1;
    dataSet.period[7].meterNum = 1;

    dataSet.period[8].mon = 1;
    dataSet.period[8].day = 1;
    dataSet.period[8].meterNum = 1;

    dataSet.period[9].mon = 1;
    dataSet.period[9].day = 1;
    dataSet.period[9].meterNum = 1;

    dataSet.period[10].mon = 1;
    dataSet.period[10].day = 1;
    dataSet.period[10].meterNum = 1;

    dataSet.period[11].mon = 1;
    dataSet.period[11].day = 1;
    dataSet.period[11].meterNum = 1;

    dataSet.period[12].mon = 1;
    dataSet.period[12].day = 1;
    dataSet.period[12].meterNum = 1;

    dataSet.period[13].mon = 1;
    dataSet.period[13].day = 1;
    dataSet.period[13].meterNum = 1;

    for (i = 0; i < 14; i++)
    {
        if ((dataSet.period[i].mon != data.period[i].mon) || 
            (dataSet.period[i].day != data.period[i].day) ||
            (dataSet.period[i].meterNum != data.period[i].meterNum))
        {
            ACSAMP_FMT_DEBUG("��������ȷ [%d]��׼:%d-%d-%d ʵ��:%d-%d-%d\n", i, dataSet.period[i].mon, dataSet.period[i].day, 
                dataSet.period[i].meterNum, data.period[i].mon, data.period[i].day, data.period[i].meterNum);
            return -7;
        }
    }

    return 0;

}

/**********************************************************************
* @name��      ac_MeterSstate_parser
* @brief��     �������״̬�ִ���
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_get_40160201_parser_js(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32    i = 0;
    AC_PRE_PROC_T *pDataMap = NULL;
    uint32    offset = 0;
    uint32    nNum = 0;
    uint16    dataLen = 0;
    uint8     tmpBuff[30];
    DAYPERIOD data = {0};
    DAYPERIOD dataSet = {0};
    
    if (get_info_preproc(oad, &pDataMap) <0)
    {
        return -1;
    }

    //ƫ�Ƶ�DATA
    offset ++;

    if (pInMsg[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    nNum = pInMsg[offset ++];
    if (nNum != 5)
    {
        return -7;
    }
    
    data.nNum = 5;
    
    for (i = 0; i < nNum; i++)
    {
        //ʱ
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            if(dataLen == 1)    //dataΪNULL
            {
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                offset += dataLen;
                continue;
            }
            
            memcpy_r((uint8 *)&data.period[i].hour, &tmpBuff[0], 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].hour[%d]", oad.value, i, data.period[i].hour);
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -4;
        }
        
        //��
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            if(dataLen == 1)    //dataΪNULL
            {
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                offset += dataLen;
                continue;
            }
            
            memcpy_r((uint8 *)&data.period[i].min, &tmpBuff[0], 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].min[%d]", oad.value, i, data.period[i].min);
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -5;
        }

        //����
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pInMsg[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            if(dataLen == 1)    //dataΪNULL
            {
                ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). index[%d] data is NULL", oad.value, i+1);
                offset += dataLen;
                continue;
            }
            
            memcpy_r((uint8 *)&data.period[i].rateNO, &tmpBuff[0], 1);
            ACSAMP_BUF_DEBUG(tmpBuff, dataLen, "oad(0x%08x). data.period[%d].rateNO[%d]", oad.value, i, data.period[i].rateNO);
            offset += dataLen;
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pInMsg[offset], inLen - offset, "oad(0x%08x)", oad.value);
            return -6;
        }
    }

    //�����׼���������ڱȽϣ���������
    dataSet.nNum = 5;
    dataSet.period[0].hour = 0;
    dataSet.period[0].min = 0;
    dataSet.period[0].rateNO = 4;
    
    dataSet.period[1].hour = 8;
    dataSet.period[1].min = 0;
    dataSet.period[1].rateNO = 2;

    dataSet.period[2].hour = 12;
    dataSet.period[2].min = 0;
    dataSet.period[2].rateNO = 3;

    dataSet.period[3].hour = 17;
    dataSet.period[3].min = 0;
    dataSet.period[3].rateNO = 2;

    dataSet.period[4].hour = 21;
    dataSet.period[4].min = 0;
    dataSet.period[4].rateNO = 3;

    for (i = 0; i < 5; i++)
    {
        if ((dataSet.period[i].hour != data.period[i].hour) || 
            (dataSet.period[i].min != data.period[i].min) ||
            (dataSet.period[i].rateNO != data.period[i].rateNO))
        {
            ACSAMP_FMT_DEBUG("��������ȷ [%d]��׼:%d-%d-%d ʵ��:%d-%d-%d\n", i, dataSet.period[i].hour, dataSet.period[i].min, 
                dataSet.period[i].rateNO, data.period[i].hour, data.period[i].min, data.period[i].rateNO);
            return -7;
        }
    }

    return 0;

}

/**********************************************************************
* @name��       ac_Get_40170200_parser
* @brief��      ���ñ�����ʱ�α�
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_Get_normal_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint8        recvBufTmp[RECV_BUF_LEN] = {0};
    int          ret = 0;
    uint32       recvLen = inLen;
    uint32       outLen = 0;
    uint32       offset = 0;
    uint8        *ptr = NULL;
    FRAME_HEAD_T tFrame = {0};

    while ((ptr = get_68ptr_from_buf(pInMsg, recvLen, &offset, &outLen)) != NULL)
    {
        memcpy(recvBufTmp, ptr, outLen);

        recvLen -= (offset + outLen);
        memmove(pInMsg, (ptr + outLen), recvLen);

        /* �Ϸ��Լ�� */
        ret = app_check_message(recvBufTmp, outLen);
        if (0 != ret)
        {
            ACSAMP_BUF_DEBUG(recvBufTmp, outLen, "app_check_message error[%d] msg recv", ret);
            continue;
        }

        /* ����֡ͷ */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        ret = app_get_frame(recvBufTmp, outLen, &tFrame, TRUE);
        if (0 != ret)
        {
            ACSAMP_BUF_DEBUG(recvBufTmp, outLen, "app_get_frame error[%d] msg recv", ret);
            continue;
        }

        ret = get_normal_with_data(&tFrame, destData, destDataBak);
        if (0 != ret)
        {
            ACSAMP_FMT_DEBUG("protocol_analyze_apdu error[%d]\n", ret);
            continue;
        }
    }

    return ret;
}

#endif

#if DESC("����ʶ������ӿ�", 1)
/**********************************************************************
* @name��       ac_Topo_parser
* @brief��      ����ʶ����
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_rpt_Topo_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32 i = 0;
    uint8 *pdata = NULL;
    uint32 nNum = 0;
    uint32 offset = 0;
    uint16 dataLen = 0;
    uint8  tmpBuff[30];
    uint16 year = 0;

    const char* sPhase[] = {"δ֪", "A��", "B��", "C��", "3��"};
     
    TOPO_IDEN_RESULT *pTopoIdenResult = (TOPO_IDEN_RESULT *)destData;

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }

    for (i = 0; i < inLen; i++)
    {
        if (i + 4 + 27 > inLen)
        {
            break;
        }
        
        if ((pInMsg[i + 0] == 0x3e) && 
            (pInMsg[i + 1] == 0x83) &&
            (pInMsg[i + 2] == 0x02) &&
            (pInMsg[i + 3] == 0x00))
        {
            pdata = &pInMsg[i + 4 + 26];
        }
    }

    if (pdata == NULL)
    {
        return ERR_RANGE;
    }

    if (pdata[offset ++] != DT_ARRAY)
    {
        return -2;
    }
    if (pdata[offset ++] != 1)
    {
        return -3;
    }
    if (pdata[offset ++] != DT_STRUCTURE)
    {
        return -4;
    }
    nNum = pdata[offset ++];
    if (nNum != 7)
    {
        return -5;
    }
    for (i = 0; i < nNum; i++)
    {
        dataLen = 0;
        if (TRUE == taskmng_oop_data_extract(&pdata[offset], &dataLen, inLen - offset, tmpBuff, DT_NULL, FALSE))
        {
            offset += dataLen;  
        }
        else
        {
            ACSAMP_BUF_DEBUG(&pdata[offset], inLen - offset, "oad(0x%08x) i(%d)", oad.value, i);
            return -6;
        }

        if (i == 0)
        {
            year = tmpBuff[0];
            year = (year << 8) + tmpBuff[1];
            pTopoIdenResult->stIdenTime.year = year;
            pTopoIdenResult->stIdenTime.month = tmpBuff[2];
            pTopoIdenResult->stIdenTime.day = tmpBuff[3];
            pTopoIdenResult->stIdenTime.hour = tmpBuff[4];
            pTopoIdenResult->stIdenTime.minute = tmpBuff[5];
            pTopoIdenResult->stIdenTime.second = tmpBuff[6];
        }
        if (i == 1)
        {
            pTopoIdenResult->nPhase = tmpBuff[0];
        }
        if (i == 2)
        {
            memcpy_r((uint8*)&pTopoIdenResult->nSignalS1, tmpBuff, 4);
        }
        if (i == 3)
        {
            memcpy_r((uint8*)&pTopoIdenResult->nSignalS2, tmpBuff, 4);
        }
        if (i == 4)
        {
            memcpy_r((uint8*)&pTopoIdenResult->nNoise, tmpBuff, 4);
        }
        if (i == 5)
        {
            memcpy((uint8*)&pTopoIdenResult->aInfo[0], tmpBuff, 2);
            membitrev((uint8*)&pTopoIdenResult->aInfo[0], 16);
            pTopoIdenResult->nInfoLen = 2;
        }
    }

    //�����¼����ϱ�
    EVENT_FMT_DEBUG("��������ʶ���¼�����¼\n");
    EVENT_FMT_DEBUG("ʶ��ɹ�ʱ��:%d-%d-%d %d:%d:%d\n", 
        pTopoIdenResult->stIdenTime.year, pTopoIdenResult->stIdenTime.month, 
        pTopoIdenResult->stIdenTime.day, pTopoIdenResult->stIdenTime.hour, 
        pTopoIdenResult->stIdenTime.minute, pTopoIdenResult->stIdenTime.second);
    EVENT_FMT_DEBUG("ʶ���ź���λ[%s]\n", pTopoIdenResult->nPhase < 5 ? sPhase[pTopoIdenResult->nPhase] : "������λ");
    EVENT_FMT_DEBUG("�ź�ǿ��1[%d]\n", pTopoIdenResult->nSignalS1);
    EVENT_FMT_DEBUG("�ź�ǿ��2[%d]\n", pTopoIdenResult->nSignalS2);
    EVENT_FMT_DEBUG("��������[%d]\n", pTopoIdenResult->nNoise);
    EVENT_BUF_DEBUG(pTopoIdenResult->aInfo, pTopoIdenResult->nInfoLen, "��������Ϣ");
    
    InitEventPara();
    SaveTopoResultRecord();
    
    return 0;
}
#endif
/*******************************************************************************
* ��������: taskmng_event_report_data_write
* ��������://����õĵ���ϱ��¼�����д����������
* �������:pipe �˿ں�
        DataBuf �¼�����
        DataLen ����
        Oad д�¼���oad

* �������:
* �� �� ֵ:
*******************************************************************************/
void acmeter_event_report_data_write(uint8* DataBuf,uint16 DataLen)
{

    int ret;
    NOMAL_OAD_T NomalOad;
    NomalOad.logicId = 0;
    NomalOad.oad.value =0xFF100A00;
    NomalOad.classtag = 5;
    NomalOad.infoNum = gAcEventNum++;
    EVENT_FMT_DEBUG( "д�¼����ݵ��������� infonum %d len%d \n",NomalOad.infoNum,DataLen);

	EVENT_FMT_DEBUG("logicId:%02x infonum:%04x oad:%08x classtag %04x:\n",NomalOad.logicId,NomalOad.infoNum,NomalOad.oad.value,NomalOad.classtag);
    EVENT_BUF_DEBUG(DataBuf, DataLen,"�ϱ��¼�����\n");
    ret = db_write_nomal(gRptClient,&NomalOad,DataBuf,DataLen);
    if(ret !=0)
    {
        EVENT_FMT_DEBUG("�¼��ϱ�����д�����ݿ�ʧ�� \n");
    }
}

/****************************************************************************
*ģ���ţ�                                                                  
*���ƣ�                                                                      
*���ܣ�                                                                      
*���������                                                                  
*���������                                                                  
*����                                                                      
*���أ���                                                                    
****************************************************************************/
BOOL acmeter_oop_next_read_data_analysis(const uint8 *inData,uint16 *offset,uint16 inDataLen,enum tag_OOP_DATATYPE DataType,BOOL bWithType)
{
    uint16 dataLen;
    uint8 nchioce;
    uint8 dataNum;
    uint8 i;
    uint16 offSet=*offset;
    enum tag_OOP_DATATYPE dataType;
    
    if(bWithType)
    {   
        dataType = DataType;
    }
    else
    {   
        dataType = (enum eDatatype)inData[offSet++];
    }
        
    if(dataType == DT_NULL) 
    {   
        *offset = offSet;
        return TRUE;
    }
    if((dataType == DT_STRUCTURE)
        ||(dataType == DT_ARRAY)
        ||(dataType == DT_OCTET_STRING)
        ||(dataType == DT_VISIBLE_STRING)
        ||(dataType == DT_UTF8_STRING)
        ||(dataType == DT_BIT_STRING)
        ||(dataType == DT_MAC)
        ||(dataType == DT_RN)
        ||(dataType == DT_TSA))
    {
        dataLen = inData[offSet++];
        if(dataLen == 0x81)
            dataLen = inData[offSet++];
        else if(dataLen == 0x82)
        {
            dataLen = inData[offSet++];
            dataLen <<= 8;
            dataLen += inData[offSet++];
        }
        else if(dataLen>0x82||dataLen==0x80)
        {      
            return FALSE;
        }
        if((dataType != DT_ARRAY) && (dataType != DT_OCTET_STRING) && (dataType != DT_VISIBLE_STRING) \
            && (dataType != DT_MAC) &&(dataLen == 0)) 
        {      
            return FALSE;
        }
        
        if(dataType == DT_BIT_STRING)
        {      
            dataLen /= 8;
        }
        
        if((dataType == DT_STRUCTURE)
            ||(dataType == DT_ARRAY))
        {
            uint16 eleNum = dataLen;
            while(eleNum--)
            {
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                {         
                    return FALSE;
                }   
            }
        }
        else
        {
            offSet+=dataLen;
        }
    }
    else if(dataType == DT_RSD)
    {
        uint8 eleNum=0;
        nchioce = inData[offSet++];
        switch(nchioce)
        {
            case 0://
                break;
            case 1:
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;
            case 2:
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                    return FALSE;
                break;  
            case 3:
                eleNum = inData[offSet++];
                for(i=0;i<eleNum;i++)
                {
                    if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_OAD,TRUE))
                        return FALSE;
                    if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
                        return FALSE;
                }
                break;
            case 4:
            case 5:
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 6:
            case 7:
            case 8:
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_TI,TRUE))
                    return FALSE;
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
            case 9:
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                break;
            case 10:
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_UNSIGNED,TRUE))
                    return FALSE;
                if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_MS,TRUE))
                    return FALSE;
                break;
                
        }
    }
    else if(dataType == DT_CSD)
    {
        nchioce = inData[offSet++];
        if(nchioce == 0)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet +=dataLen;
        }
        else if(nchioce == 1)
        {
            dataLen = data_type_length_get(DT_OAD);
            offSet +=dataLen;
            dataNum = inData[offSet++];
            offSet +=dataLen*dataNum;
        }
        else 
            return FALSE;

    }
    else if(dataType == DT_ROAD)
    {
        dataLen = data_type_length_get(DT_OAD);
        offSet +=dataLen;
        dataNum = inData[offSet++];
        offSet +=dataLen*dataNum;
        printf("ROAD LEN = %d\n",dataLen+1+dataLen*dataNum);
    }
    else if(dataType == DT_RCSD)
    {
        dataNum = inData[offSet++];
        for(i=0;i<dataNum;i++)
        {
            if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_CSD,TRUE))
                return FALSE;
        }
    }
    else if(dataType == DT_MS)
    {
        nchioce = inData[offSet++];
        if((nchioce == 0)||(nchioce == 1))
        {
        
        }
        else if(nchioce == 2)//һ���û�����
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 3)
        {
            dataNum = inData[offSet++];
            while(dataNum--)
            {
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet += dataLen;
            }
        }
        else if(nchioce == 4)
        {
            dataNum = inData[offSet++];
            dataLen = data_type_length_get(DT_LONG_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 5)//һ���û���������
        {
            dataNum = inData[offSet++];
            
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else if(nchioce == 6)//һ���û���ַ����
        {
            dataNum = inData[offSet++];         
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
            }
        }
        else if(nchioce == 7)//һ�������������
        {
            dataNum = inData[offSet++];
            
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = data_type_length_get(DT_LONG_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else
            return FALSE;

    }
    else if(dataType == DT_REGION)
    {
        if(inData[offSet++]>3)
            return FALSE;
        if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
        if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_NULL,FALSE))
            return FALSE;
    }
    else if(dataType == DT_SID)
    {
        if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_DOUBLE_LONG_UNSIGNED,TRUE))
            return FALSE;
        if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_OCTET_STRING,TRUE))
            return FALSE;   
    }
    else if(dataType == DT_MAC)
    {
        if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_SID,TRUE))
            return FALSE;
        if(FALSE == acmeter_oop_next_read_data_analysis(inData,&offSet,inDataLen,DT_MAC,TRUE))
            return FALSE;
    }
    else if(dataType == DT_TI)
    {
        if(inData[offSet++]>5)
            return FALSE;
        offSet += 2;
    }
    else
    {
        
        dataLen = data_type_length_get(dataType);
        if(0xFF != dataLen) 
            offSet +=dataLen;
        else
            return FALSE;
    }
    if(offSet>inDataLen)
        return FALSE;
    *offset = offSet;
    return TRUE;
}

/*******************************************************************************
* ��������: ac_event_report_data_save
* ��������://��ͨ��8802�����ϱ����¼�д����������
* �������:pipe �˿ں�
        DataBuf �¼����� 30 11 02 01 07 00 20 22 02 00 00 20 1e 02 00 00 20 20 02 00 00 20 24 02 00 00 33 00 02 00 00 00 10 22 01 00 00 20 22 01 01 01 06 00 00 00 02 1c 07 e5 03 1f 13 38 0a 1c 07 e5 04 01 09 1b 36 00 00 06 00 00 00 00 06 00 00 00 00
        buflen ����
        pn ���

* �������:
* �� �� ֵ: �����ı��ĳ���
*******************************************************************************/

uint16 ac_event_report_data_save(uint8 *DataBuf,uint16 DataLen)
{
    uint8 i,j,k;
    uint16 index=0;
    uint8 CsdNum;
    uint8 RecordNum;
    time_t        RealTime_t = time(NULL);
    //uint8 Addr[6]={0};
    uint16 rptLen = 25;
    //�¼��ϱ�ʱ�ѹ̶���CSD�����ȥ
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};

    rptBuff[rptLen++] = 1;  //CSD����1 ��¼�Ͷ���������
    EVENT_FMT_DEBUG( "event_report_data_save \n");
    EVENT_BUF_DEBUG(DataBuf, DataLen,"���ɵװ��ϱ�����\n");



    memcpy(&rptBuff[rptLen],DataBuf,4);
    rptLen+=4;
    index+=4;
    CsdNum = DataBuf[index++];
    rptBuff[rptLen++] = CsdNum;
    for(i=0;i<CsdNum;i++)
    {
        if(DataBuf[index++]!=0)
            return index;
        memcpy(&rptBuff[rptLen],&DataBuf[index],4);
        index+=4;
        rptLen+=4;
    }
    if(DataBuf[index++]==0)   //0Ϊ����DAR 1 ΪSEQUENCE OF A-RecordRow
        return index;
    RecordNum = DataBuf[index++];
    if(RecordNum==0)
        return index;

    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = RecordNum;
    for(j=0;j<RecordNum;j++)
    {
        //��̶�������
        //��������ַ
        rptBuff[rptLen++] = DT_TSA;
        rptBuff[rptLen++] = 7;
        rptBuff[rptLen++] = 5;
        //taskmng_mtr_addr_get( Pn,LOGIC_ID_YC,Addr);
        memcpy(&rptBuff[rptLen],tmnAddr.value, 6);
        rptLen+=6;
        //����ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //�ɹ�ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;
        //�洢ʱ��
        rptBuff[rptLen++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
        rptLen+=7;

        rptBuff[rptLen++] = 1;
        rptBuff[rptLen++] = CsdNum;

        uint16 indexbak = index;
        for(k=0;k<CsdNum;k++)
        {
            acmeter_oop_next_read_data_analysis(DataBuf,&index,DataLen,DT_NULL,FALSE);
        }
        memcpy(&rptBuff[rptLen],&DataBuf[indexbak],index-indexbak);
        rptLen+=(index-indexbak);
      

    }
    acmeter_event_report_data_write(rptBuff,rptLen);
    return index;
}

/**********************************************************************
* @name��       ac_Topo_parser
* @brief��      ����ʶ����
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
int ac_rpt_meter_event_parser(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak)
{
    uint32 i = 0;
    uint32 nNum = 0;
    uint32 offset = 0;
    uint32 index=0;
    
    InitEventRptPara();

    if (destData == NULL)
    {
        ACSAMP_FMT_DEBUG("destData err oad(0x%08x). type[0x%x]. destData[p%]\n", 
            oad.value, pInMsg[offset], destData);
        return -1;
    }
    if (pInMsg == NULL)
    {
        return ERR_RANGE;
    }

    if (pInMsg[index ++] != 0x88)
    {
        return -2;
    }
    if (pInMsg[index ++] != 0x02)
    {
        return -3;
    }
    index++;
    nNum = pInMsg[index ++];

    for (i = 0; i < nNum; i++)
    {
        offset = ac_event_report_data_save(pInMsg+index,inLen-index);
        index +=offset;
    } 
    return 0;
}



