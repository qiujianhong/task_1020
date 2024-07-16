/*
*********************************************************************
* @file    class13.c
* @brief�� class13����
* @author��
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "macro.h"
#include "oopVarUpdate.h"
#include "oopStorage.h"

//
// class13
//
extern void appmain_buzzer_status(int dbClient, uint8 status);
extern void CheckERC_3202(int fd, OI SourceOI, OOP_CFGUNIT_810C_T cfgunit);
typedef uint8 (*CTRL_SHOW_HINT)(uint8, uint8, uint8, uint8*, uint16, uint16*);
extern void appctrl_alarm_data(uint8 ctrltype, uint8 group, uint8 times, uint8 Voindex, CTRL_SHOW_HINT pfunc);

#if DESC("����ͨ�ýӿ�", 1)
/**
*********************************************************************
* @brief��     �ܼ�OI
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_comm_oi(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OI data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data, sizeof(OI));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    
    
    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
#endif

#if DESC("����Ͷ��ͽ��",1)
/**
*********************************************************************
* @brief��      ���ݿ�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_ctl_on_off(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OI data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data, sizeof(OI));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    
    
    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
#endif

#if DESC("����Ͷ��Ϳ������״̬",1)
/**
*********************************************************************
* @brief��      ����Ͷ��״̬
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_ctl_in_state(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TGSTATES_T data;
//    uint8 offlen = 0;      //������������
//    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint16 numoffset = 0;
    uint8  num = 0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    numoffset = dOffset;
    dstBuf[dOffset++] = 0;

    for(i = 0; i < data.nNum; i++)
    {
        if(data.item[i].obj > 0x2308 || data.item[i].obj < 0x2301)
        {
            continue;
        }
        num++;
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 2;
                
        //�ܼ������
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.item[i].obj, sizeof(OI));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),obj(0x%x)\n", i, dar, data.item[i].obj);
            return DATA_TYPE_UNMATCHED;
        }
        
        //״̬
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.item[i].state, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),state(0x%x)\n", i, dar, data.item[i].state);
            return DATA_TYPE_UNMATCHED;
        }      
    }
    dstBuf[numoffset] = num;

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
* @brief��      �������״̬
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset    ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_ctl_out_state(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TURNSTATES_T data;
//    uint8 offlen = 0;      //������������
//    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint16 numoffset = 0;
    uint8  num = 0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    numoffset = dOffset;
    dstBuf[dOffset++] = 0;
    
    for(i = 0; i < data.nNum; i++)
    {
        if(data.item[i].obj > 0x2308 || data.item[i].obj < 0x2301)
        {
            continue;
        }
        num++;
        
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 2;
                
        //�ܼ������
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.item[i].obj, sizeof(OI));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(0x%x)\n", i, dar, data.item[i].obj);
            return DATA_TYPE_UNMATCHED;
        }
        
        //ÿ���޵���
        dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.item[i].state, sizeof(OOP_BITSTR8_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),powLimit(0x%x)\n", i, dar, data.item[i].state);
            return DATA_TYPE_UNMATCHED;
        }      
    }
    dstBuf[numoffset] = num;

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
* @brief��      ���Ƹ澯״̬
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset    ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_ctl_alarm_state(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_TGSTATES_T data;
//    uint8 offlen = 0;      //������������
//    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint16 numoffset = 0;
    uint8  num = 0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    numoffset = dOffset;
    dstBuf[dOffset++] = 0;
    
    for(i = 0; i < data.nNum; i++)
    {
        if(data.item[i].obj > 0x2308 || data.item[i].obj < 0x2301)
        {
            continue;
        }
        num++;
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 2;
                
        //�ܼ������
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.item[i].obj, sizeof(OI));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(0x%x)\n", i, dar, data.item[i].obj);
            return DATA_TYPE_UNMATCHED;
        }
        
        //״̬
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.item[i].state, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),powLimit(0x%x)\n", i, dar, data.item[i].state);
            return DATA_TYPE_UNMATCHED;
        }      
    }

    dstBuf[numoffset] = num;

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

#endif

#if DESC("ʱ�ο����ת������",1)
/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(ʱ�οض�ֵ����)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_power_ctrl_param(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint32     i = 0;
    POWERCTRLPARAM data;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&data, srcData, srcLen);

    //�ṹ����
    dstBufTmp[offset++] = DT_STRUCTURE;

    //�ṹ��Ա����
    dstBufTmp[offset++] = 9;    

    //ʱ�κ�
    dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBufTmp, dstLen-offset, &offset, &data.valid, sizeof(data.valid));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
        return dar;
    } 

    //ʱ�ι��ض�ֵ
    for (i = 0; i < OOP_MAX_TURN; i++)
    {
        dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen-offset, &offset, &data.defValue[i], sizeof(data.defValue[i]));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
            return dar;
        } 
    }

    *pOffset = offset;

    return ERR_OK;
}

/**
*********************************************************************
* @brief��      ʱ�ο�
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_cfg_8103(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8103_CONFIG_T data;
//    uint8 offlen = 0;      //������������
//    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint16 numoffset = 0;
    uint8  num = 0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    numoffset = dOffset;
    dstBuf[dOffset++]  = 0;

    for(i = 0; i < data.nNum; i++)
    {
        if(data.item[i].object > 0x2308 || data.item[i].object < 0x2301)
        {
            continue;
        }
        num++;
        
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 6;
                
        //�ܼ������
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.item[i].object, sizeof(data.item[i].object));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(0x%x)\n", i, dar, data.item[i].object);
            return DATA_TYPE_UNMATCHED;
        }
        
        //������ʶ
        dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.item[i].plan, sizeof(data.item[i].plan));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.item[i].plan);
            return DATA_TYPE_UNMATCHED;
        }

        //3�׶�ֵ
        for (j = 0; j < 3; j++)
        {
            data_to_buf_power_ctrl_param(pOadInfo, dstBuf, dstLen, &dOffset, &data.item[i].value[j], sizeof(data.item[i].value[j]));
            if (dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("data_to_buf_power_ctrl_param failed. [%d] dar(%d)\n", j, dar);
                return DATA_TYPE_UNMATCHED;
            }
        }
        dstBuf[numoffset]  = num;

        //ʱ�ι��ض�ֵ����ϵ��
        dar = basic_data_to_buf(E_INTEGER, dstBuf, dstLen, &dOffset, &data.item[i].floatCoe, sizeof(data.item[i].floatCoe));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),floatCoe(%d)\n", i, dar, data.item[i].floatCoe);
            return DATA_TYPE_UNMATCHED;
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
* @brief��     ��698��ʽ����ת�������ĵ�����(ʱ�οض�ֵ����)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
               !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_power_ctrl_param(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *pOffset;
    uint32  i = 0;    
    POWERCTRLPARAM data;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //�ṹ����
    if(srcBufTmp[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ա��
    if(srcBufTmp[sOffset++] != 9)
    {
        PRTL_BUF_DEBUG(srcBufTmp, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //ʱ�κ�
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data.valid, sizeof(data.valid));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //ʱ�ι��ض�ֵ
    for (i = 0; i < OOP_MAX_TURN; i++)
    {
        dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.defValue[i], sizeof(data.defValue[i]));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }
    }

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if(sOffset > srcLen)
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d)\n", sOffset, srcLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    memcpy(dstData, &data, dstLen);

    *pOffset = sOffset;
  
    return dar;
}

/**
*********************************************************************
* @brief��      ʱ�ο�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8103(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8103_CONFIG_T data;
    uint8 offlen = 0;      //������������
    uint32 i = 0;
    uint32 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    if (data.nNum > OOP_MAX_TG)
    {
        data.nNum = OOP_MAX_TG;
    }
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //�ṹ��
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ṹ��Ԫ������
        if(srcBuf[sOffset++] != 6)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ܼ������
        dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.item[i].object, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        //������ʶ
        dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data.item[i].plan, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //ʱ�οض�ֵ����
        for (j = 0; j < 3; j++)
        {
            dar = buf_to_data_power_ctrl_param(pOadInfo, srcBuf, srcLen, &sOffset, &data.item[i].value[j], sizeof(POWERCTRLPARAM));
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("buf_to_data_power_ctrl_param failed. [%d] dar(%d)\n", j, dar);
                return DATA_TYPE_UNMATCHED;
            }
        }

        //ʱ�ι��ض�ֵ����ϵ��
        dar = basic_buf_to_data(E_INTEGER, srcBuf, srcLen, &sOffset, &data.item[i].floatCoe, sizeof(data.item[i].floatCoe));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }  
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��      ʱ�ο�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8103_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CFGUNIT_8109_T data;
    uint32 i = 0;
    uint32 j = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 6)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //������ʶ
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data.plan, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //ʱ�οض�ֵ����
    for (j = 0; j < 3; j++)
    {
        dar = buf_to_data_power_ctrl_param(pOadInfo, srcBuf, srcLen, &sOffset, &data.value[j], sizeof(POWERCTRLPARAM));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("buf_to_data_power_ctrl_param failed. [%d] dar(%d)\n", j, dar);
            return DATA_TYPE_UNMATCHED;
        }
    }

    //ʱ�ι��ض�ֵ����ϵ��
    dar = basic_buf_to_data(E_INTEGER, srcBuf, srcLen, &sOffset, &data.floatCoe, sizeof(data.floatCoe));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��      ʱ�ο�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_ctl_8103_switch(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_POWCON_PERIOD_T data;
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //ʱ�ι���Ͷ���ʶ
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data.plan.flag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //ʱ�ι��ض�ֵ������
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.plan.planID, sizeof(data.plan.planID));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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

#endif

#if DESC("���ݿ����ת������",1)
/**
*********************************************************************
* @brief��      ���ݿ�
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_cfg_8104(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8104_CONFIG_T data;
//    uint8 offlen = 0;      //������������
//    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint16 numoffset = 0;
    uint8  num = 0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    numoffset = dOffset;
    dstBuf[dOffset++] = 0;

    for(i = 0; i < data.nNum; i++)
    {
        if(data.item[i].object > 0x2308 || data.item[i].object < 0x2301)
        {
            continue;
        }
        num++;
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 5;
                
        //�ܼ������
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.item[i].object, sizeof(OI));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(0x%x)\n", i, dar, data.item[i].object);
            return DATA_TYPE_UNMATCHED;
        }

        //���ݿض�ֵ
        dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.item[i].defValue, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),floatCoe(%d)\n", i, dar, data.item[i].defValue);
            return DATA_TYPE_UNMATCHED;
        }  

        //�޵���ʼʱ��
        dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.item[i].startTime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),startTime.year(%d)\n", i, dar, data.item[i].startTime.year);
            return DATA_TYPE_UNMATCHED;
        }  

        //�޵�����ʱ��
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].duration, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),duration(%d)\n", i, dar, data.item[i].duration);
            return DATA_TYPE_UNMATCHED;
        }  
        
        //ÿ���޵���
        dar = basic_data_to_buf(E_OOP_BITSTR8_T, dstBuf, dstLen, &dOffset, &data.item[i].powLimit, sizeof(OOP_BITSTR8_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),powLimit(0x%x)\n", i, dar, data.item[i].powLimit);
            return DATA_TYPE_UNMATCHED;
        }      
    }
    dstBuf[numoffset] = num;

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
* @brief��      ���ݿ�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8104(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8104_CONFIG_T data;
    uint8 offlen = 0;      //������������
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    if (data.nNum > OOP_MAX_TG)
    {
        data.nNum = OOP_MAX_TG;
    }
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //�ṹ��
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ṹ��Ԫ������
        if(srcBuf[sOffset++] != 5)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ܼ������
        dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.item[i].object, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        //���ݿض�ֵ
        dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.item[i].defValue, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //�޵���ʼʱ��
        dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.item[i].startTime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //�޵�����ʱ��
        dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.item[i].duration, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }  

        //ÿ���޵���
        dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data.item[i].powLimit, sizeof(OOP_BITSTR8_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��     ���ݿ�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8104_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CFGUNIT_810A_T data;
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 5)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //���ݿض�ֵ
    dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.defValue, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //�޵���ʼʱ��
    dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.startTime, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    data.startTime.year = 0xffff;
    data.startTime.month = 0xff;
    data.startTime.day = 0xff;

    //�޵�����ʱ��
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.duration, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //ÿ���޵���
    dar = basic_buf_to_data(E_OOP_BITSTR8_T, srcBuf, srcLen, &sOffset, &data.powLimit, sizeof(OOP_BITSTR8_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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

#endif


#if DESC("Ӫҵ��ͣ�����ת������",1)
/**
*********************************************************************
* @brief��      Ӫҵ��ͣ��
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_cfg_8105(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8105_CONFIG_T data;
//    uint8 offlen = 0;      //������������
//    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint16 numoffset = 0;
    uint8  num = 0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    numoffset = dOffset;
    dstBuf[dOffset++] = 0;

    for(i = 0; i < data.nNum; i++)
    {
        if(data.item[i].object > 0x2308 || data.item[i].object < 0x2301)
        {
            continue;
        }
        num++;
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 4;
                
        //�ܼ������
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.item[i].object, sizeof(OI));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(0x%x)\n", i, dar, data.item[i].object);
            return DATA_TYPE_UNMATCHED;
        }

        //��ͣ��ʼʱ��
        dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.item[i].startTime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),startTime.year(%d)\n", i, dar, data.item[i].startTime.year);
            return DATA_TYPE_UNMATCHED;
        }  

        //��ͣ����ʱ��
        dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.item[i].endTime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),endTime.year(%d)\n", i, dar, data.item[i].endTime.year);
            return DATA_TYPE_UNMATCHED;
        }  
        
        //��ͣ�ع��ʶ�ֵ
        dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.item[i].defValue, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),floatCoe(%d)\n", i, dar, data.item[i].defValue);
            return DATA_TYPE_UNMATCHED;
        }        
    }
    dstBuf[numoffset] = num;

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
* @brief��     Ӫҵ��ͣ��
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8105(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8105_CONFIG_T data;
    uint8 offlen = 0;      //������������
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    if (data.nNum > OOP_MAX_TG)
    {
        data.nNum = OOP_MAX_TG;
    }
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //�ṹ��
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ṹ��Ԫ������
        if(srcBuf[sOffset++] != 4)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ܼ������
        dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.item[i].object, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        //��ͣ��ʼʱ��
        dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.item[i].startTime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //��ͣ����ʱ��
        dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.item[i].endTime, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }   

        //��ͣ�ع��ʶ�ֵ
        dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.item[i].defValue, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��     Ӫҵ��ͣ��
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8105_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CFGUNIT_810B_T data;
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 4)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //��ͣ��ʼʱ��
    dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.startTime, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
    data.startTime.hour = 0xff;
    data.startTime.minute = 0xff;

    //��ͣ����ʱ��
    dar = basic_buf_to_data(E_DATETIME_S, srcBuf, srcLen, &sOffset, &data.endTime, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   
    data.endTime.hour = 0xff;
    data.endTime.minute = 0xff;

    //��ͣ�ع��ʶ�ֵ
    dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.defValue, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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

#endif

#if DESC("�¸������ת������",1)
/**
*********************************************************************
* @brief��      ʱ�ο�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_ctl_powcon_low(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_POWCON_LOW_T data;
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 8)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //��ǰ�����¸��ն�ֵ����ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.slipTime, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ض�ֵ����ϵ��
    dar = basic_buf_to_data(E_INTEGER, srcBuf, srcLen, &sOffset, &data.scheme.floatCoefficient, sizeof(int8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //�غ��ܼ��й����ʶ�����ʱʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.delayTime, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��صĿ���ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.conTime, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[0], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[1], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[2], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[3], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��      ʱ�ο�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_ctl_jiangsu_powcon_low(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_POWCON_LOW_T data;
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 0x0c)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //��ǰ�����¸��ն�ֵ����ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.slipTime, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ض�ֵ����ϵ��
    dar = basic_buf_to_data(E_INTEGER, srcBuf, srcLen, &sOffset, &data.scheme.floatCoefficient, sizeof(int8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //�غ��ܼ��й����ʶ�����ʱʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.delayTime, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��صĿ���ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.conTime, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[0], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[1], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[2], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[3], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
	 //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[4], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
	//��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[5], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
	//��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[6], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
	//��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[7], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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

#endif
/**
*********************************************************************
* @brief��      ʱ�ο�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_ctl_powcon_low_extend(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_POWCON_LOW_T data;
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 2)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 0x0c)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //��ǰ�����¸��ն�ֵ����ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.slipTime, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ض�ֵ����ϵ��
    dar = basic_buf_to_data(E_INTEGER, srcBuf, srcLen, &sOffset, &data.scheme.floatCoefficient, sizeof(int8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }  

    //�غ��ܼ��й����ʶ�����ʱʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.delayTime, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��صĿ���ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.conTime, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[0], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[1], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[2], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��ǰ�����¸��ظ澯ʱ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[3], sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 
	
	//��ǰ�����¸��ظ澯ʱ��
   dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[0], sizeof(uint8));
   if(dar != DATA_SUCCESS)
   {
	   PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
	   return DATA_TYPE_UNMATCHED;
   } 

   //��ǰ�����¸��ظ澯ʱ��
   dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[1], sizeof(uint8));
   if(dar != DATA_SUCCESS)
   {
	   PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
	   return DATA_TYPE_UNMATCHED;
   } 

   //��ǰ�����¸��ظ澯ʱ��
   dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[2], sizeof(uint8));
   if(dar != DATA_SUCCESS)
   {
	   PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
	   return DATA_TYPE_UNMATCHED;
   } 

   //��ǰ�����¸��ظ澯ʱ��
   dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.scheme.waringTime[3], sizeof(uint8));
   if(dar != DATA_SUCCESS)
   {
	   PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
	   return DATA_TYPE_UNMATCHED;
   } 
    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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

#if DESC("��������ת������",1)
/**
*********************************************************************
* @brief��      ����ش���
* @param[in] �� fd           - �������ľ��
                new810CDate  - �·��Ĺ������
* @param[out] ��latestRemain - �����һ��ʣ�����
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E remain_energy_exec(DB_CLIENT fd, int64* latestRemain, OOP_CFGUNIT_810C_T new810CDate, int64* newRemain)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    int32 ret = ERR_OK;
    uint32 vir_no = 0;
    int64 remainEN = 0;

    if (latestRemain == NULL)
    {
        return DATA_DATA_BLOCK_UNAVAILABLE;
    }

    vir_no = new810CDate.object - 0x2301;

    //���緽ʽ��ˢ�º�׷��
    if (new810CDate.flag == 1)
    {
        remain_energy_read(fd, vir_no + 1, latestRemain);
        *newRemain = new810CDate.purElectricity;
        ret = remain_energy_write(fd, vir_no + 1, new810CDate.purElectricity);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("remain_energy_write err ret[%d]\n", ret);
            return DATA_READ_WRITE_DENIED;
        }
    }
    else if (new810CDate.flag == 0)
    {
        ret = remain_energy_read(fd, vir_no + 1, &remainEN);
        if (ret != ERR_OK)
        {
            /* û������Ĭ����0 */
            remainEN = 0;
        }

        *latestRemain = remainEN;

        remainEN += new810CDate.purElectricity;
        if (remainEN > 99999990000000LL)
        {
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }
        *newRemain = remainEN;

        ret = remain_energy_write(fd, vir_no + 1, remainEN);
        if (ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("remain_energy_write err ret[%d]\n", ret);
            return DATA_READ_WRITE_DENIED;
        }
    }

    return dar;
}

/**
*********************************************************************
* @brief��       �����
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_cfg_810C(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CFGUNIT_810C_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //�ṹ����
    dstBuf[dOffset++] = DT_STRUCTURE;

    //�ṹ��Ա��
    dstBuf[dOffset++] = 8;
            
    //�ܼ������
    dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.object, sizeof(OI));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),object(0x%x)\n", dar, data.object);
        return DATA_TYPE_UNMATCHED;
    }

    //���絥��
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.number, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),number(%d)\n", dar, data.number);
        return DATA_TYPE_UNMATCHED;
    }  

    //׷��/ˢ�±�ʶ
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.flag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),flag(%d)\n", dar, data.flag);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //��������
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.type, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),flag(%d)\n", dar, data.type);
        return DATA_TYPE_UNMATCHED;
    }  

    //������(��)ֵ
    dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.purElectricity, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),purElectricity(%d)\n", dar, data.purElectricity);
        return DATA_TYPE_UNMATCHED;
    }  

    //��������ֵ
    dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.alarmLimit, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),alarmLimit(%d)\n", dar, data.alarmLimit);
        return DATA_TYPE_UNMATCHED;
    } 

    //��բ����ֵ
    dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.tripLimit, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),alarmLimit(%d)\n", dar, data.tripLimit);
        return DATA_TYPE_UNMATCHED;
    } 

    //��������
    dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.mode, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),flag(%d)\n", dar, data.mode);
        return DATA_TYPE_UNMATCHED;
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
* @brief��      �����
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_cfg_8107(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    uint32  numoffset = 0;
    uint32   num = 0;
    OOP_8107_CONFIG_T data;
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    numoffset = dOffset;
    dOffset++;

    for(i = 0; i < data.nNum; i++)
    {
        if(data.item[i].object < 0x2301 || data.item[i].object > 0x2308)
        {
            continue;
        }
        num++;
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 8;
                
        //�ܼ������
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.item[i].object, sizeof(OI));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(0x%x)\n", i, dar, data.item[i].object);
            return DATA_TYPE_UNMATCHED;
        }

        //���絥��
        dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].number, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),number(%d)\n", i, dar, data.item[i].number);
            return DATA_TYPE_UNMATCHED;
        }  

        //׷��/ˢ�±�ʶ
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.item[i].flag, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),flag(%d)\n", i, dar, data.item[i].flag);
            return DATA_TYPE_UNMATCHED;
        }  
        
        //��������
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.item[i].type, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),flag(%d)\n", i, dar, data.item[i].type);
            return DATA_TYPE_UNMATCHED;
        }  

        //������(��)ֵ
        dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.item[i].purElectricity, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),purElectricity(%d)\n", i, dar, data.item[i].purElectricity);
            return DATA_TYPE_UNMATCHED;
        }  

        //��������ֵ
        dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.item[i].alarmLimit, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),alarmLimit(%d)\n", i, dar, data.item[i].alarmLimit);
            return DATA_TYPE_UNMATCHED;
        } 

        //��բ����ֵ
        dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.item[i].tripLimit, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),alarmLimit(%d)\n", i, dar, data.item[i].tripLimit);
            return DATA_TYPE_UNMATCHED;
        } 

        //��������
        dar = basic_data_to_buf(E_ENUM, dstBuf, dstLen, &dOffset, &data.item[i].mode, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),flag(%d)\n", i, dar, data.item[i].mode);
            return DATA_TYPE_UNMATCHED;
        }  
             
    }
    dstBuf[numoffset] = num;

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
* @brief��     �����
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8107(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8107_CONFIG_T data;
    uint8 offlen = 0;      //������������
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 num = 0;
    OOP_CFGUNIT_810C_T  item;
    MEMZERO(&data, sizeof(data));

    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    num = get_len_offset(&srcBuf[sOffset], &offlen);
    data.nNum = num;
    sOffset += offlen;

    for(i = 0; i < num; i++)
    {
        memset(&item, 0, sizeof(OOP_CFGUNIT_810C_T));
        //�ṹ��
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ṹ��Ԫ������
        if(srcBuf[sOffset++] != 8)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ܼ������
        dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &item.object, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        if(item.object > 0x2308 || item.object < 0x2301)
        {
            PRTL_FMT_DEBUG("Խ�磬 item.object = %08x\n", item.object);
            return DATA_SCOPE_OF_ACCESS_VIOLATED;
        }
        //���絥��
        dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &item.number, sizeof(uint32));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //׷��/ˢ�±�ʶ
        dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &item.flag, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }   

        //��������
        dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &item.type, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //������(��)ֵ
        dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &item.purElectricity, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        //��������ֵ
        dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &item.alarmLimit, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //��բ����ֵ
        dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &item.tripLimit, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }   

        //�����ģʽ
        dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &item.mode, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        data.item[i] = item;
    }
    
    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��     �����
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8107_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CFGUNIT_810C_T data;
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 8)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //���絥��
    dar = basic_buf_to_data(E_DOUBLE_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.number, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //׷��/ˢ�±�ʶ
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.flag, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    //��������
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.type, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //������(��)ֵ
    dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.purElectricity, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //��������ֵ
    dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.alarmLimit, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��բ����ֵ
    dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.tripLimit, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    //�����ģʽ
    dar = basic_buf_to_data(E_ENUM, srcBuf, srcLen, &sOffset, &data.mode, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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

#endif

#if DESC("�µ�����ת������",1)
/**
*********************************************************************
* @brief��      �µ��
* @param[in] �� srcBuf     Դ��������ͨ����һ��������ַ
*               srcLen     Դ���ݳ��ȣ�ͨ���Ǳ�����С
*               dstLen     Ŀ�����ݳ��ȣ�698���ĵ�buffer����

* @param[out] ��pOffset     ��������ݺ��Ŀ��������ƫ��
*               dstData    Ŀ����������698����buffer
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_cfg_8108(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8108_CONFIG_T data;
//    uint8 offlen = 0;      //������������
//    uint8 lenBuf[3] = {0}; //������������
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint16 numoffset = 0;
    uint8  num = 0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //��������
    dstBuf[dOffset++] = DT_ARRAY;

    //�����Ա����
    numoffset = dOffset;
    dstBuf[dOffset++] = 0;

    for(i = 0; i < data.nNum; i++)
    {
        if(data.item[i].object < 0x2301 || data.item[i].object > 0x2308)
        {
            continue;
        }
        num++;
        //�ṹ����
        dstBuf[dOffset++] = DT_STRUCTURE;

        //�ṹ��Ա��
        dstBuf[dOffset++] = 4;
                
        //�ܼ������
        dar = basic_data_to_buf(E_OI, dstBuf, dstLen, &dOffset, &data.item[i].object, sizeof(OI));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),object(0x%x)\n", i, dar, data.item[i].object);
            return DATA_TYPE_UNMATCHED;
        }

        //�µ����ض�ֵ
        dar = basic_data_to_buf(E_LONG64, dstBuf, dstLen, &dOffset, &data.item[i].defValue, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),alarmLimit(%d)\n", i, dar, data.item[i].defValue);
            return DATA_TYPE_UNMATCHED;
        } 

        //��������ֵϵ��
        dar = basic_data_to_buf(E_UNSIGNED, dstBuf, dstLen, &dOffset, &data.item[i].limitCoe, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),number(%d)\n", i, dar, data.item[i].limitCoe);
            return DATA_TYPE_UNMATCHED;
        }  

        //�µ�ض�ֵ����ϵ��
        dar = basic_data_to_buf(E_INTEGER, dstBuf, dstLen, &dOffset, &data.item[i].floatCoe, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),flag(%d)\n", i, dar, data.item[i].floatCoe);
            return DATA_TYPE_UNMATCHED;
        }         
    }

    dstBuf[numoffset] = num;
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
* @brief��     �µ��
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8108(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_8108_CONFIG_T data;
    uint8 offlen = 0;      //������������
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    if (data.nNum > OOP_MAX_TG)
    {
        data.nNum = OOP_MAX_TG;
    }
    sOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //�ṹ��
        if(srcBuf[sOffset++] != DT_STRUCTURE)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ṹ��Ԫ������
        if(srcBuf[sOffset++] != 4)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
            return DATA_TYPE_UNMATCHED;
        }

        //�ܼ������
        dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.item[i].object, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }    

        //�µ����ض�ֵ
        dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.item[i].defValue, sizeof(int64));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 

        //��������ֵϵ��
        dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.item[i].limitCoe, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        }   

        //�µ�ض�ֵ����ϵ��
        dar = basic_buf_to_data(E_INTEGER, srcBuf, srcLen, &sOffset, &data.item[i].floatCoe, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
            return DATA_TYPE_UNMATCHED;
        } 
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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
* @brief��     �µ��
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_cfg_8108_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_CFGUNIT_810D_T data;
    uint32 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));

    //�ṹ��
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ṹ��Ԫ������
    if(srcBuf[sOffset++] != 4)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�ܼ������
    dar = basic_buf_to_data(E_OI, srcBuf, srcLen, &sOffset, &data.object, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }    

    //�µ����ض�ֵ
    dar = basic_buf_to_data(E_LONG64, srcBuf, srcLen, &sOffset, &data.defValue, sizeof(int64));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    //��������ֵϵ��
    dar = basic_buf_to_data(E_UNSIGNED, srcBuf, srcLen, &sOffset, &data.limitCoe, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }   

    //�µ�ض�ֵ����ϵ��
    dar = basic_buf_to_data(E_INTEGER, srcBuf, srcLen, &sOffset, &data.floatCoe, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. [%d] dar(%d), sOffset(%d)\n", i, dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    } 

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

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

#endif

/**
*********************************************************************
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_act_8103(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_OMD_U omd = {0};

    memcpy(&omd, &pOadInfo->pTab->oad, sizeof(uint32));

    switch (omd.nModID)
    {
        case 0x03:
        case 0x05:
        {
            return buf_to_data_cfg_8103_unit(pOadInfo,  convert);
        }break;

        case 0x04:
        {
            return buf_to_data_comm_oi(pOadInfo,  convert);
        }break; 

        case 0x07:
        case 0x06:   //����Ͷ��ͽ��
        {
            return buf_to_data_ctl_on_off(pOadInfo,  convert);
        }break;
     
        case 0x7F:
        {
            return buf_to_data_ctl_8103_switch(pOadInfo,  convert);
        }break;
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_act_8104(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x03:
        case 0x05:
        {
            return buf_to_data_cfg_8104_unit(pOadInfo,  convert);
        }break; 

        case 0x04:
        {
            return buf_to_data_comm_oi(pOadInfo,  convert);
        }break;

        case 0x07:
        case 0x06:   //����Ͷ��ͽ��
        {
            return buf_to_data_ctl_on_off(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_act_8105(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x03:
        case 0x05:
        {
            return buf_to_data_cfg_8105_unit(pOadInfo,  convert);
        }break; 

        case 0x04:
        {
            return buf_to_data_comm_oi(pOadInfo,  convert);
        }break; 

        case 0x07:
        case 0x06:   //����Ͷ��ͽ��
        {
            return buf_to_data_ctl_on_off(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_act_8106(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_OMD_U omd = {0};

    memcpy(&omd, &pOadInfo->pTab->oad, sizeof(uint32));

    switch (omd.nModID)
    {
        case 0x07:
        case 0x06:   //����Ͷ��ͽ��
        {
            return buf_to_data_ctl_on_off(pOadInfo,  convert);
        }break;
        
        case 0x7F:  //����Ͷ��ͽ��
        {
            return buf_to_data_ctl_powcon_low(pOadInfo,  convert);
        }break;
		case 0xD9:  //����Ͷ��ͽ��
        {
            return buf_to_data_ctl_jiangsu_powcon_low(pOadInfo,  convert);
        }break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_act_8107(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x03:
        case 0x05:
        {
            return buf_to_data_cfg_8107_unit(pOadInfo,  convert);
        }break; 

        case 0x04:
        {
            return buf_to_data_comm_oi(pOadInfo,  convert);
        }break; 
        
        case 0x07:
        case 0x06:   //����Ͷ��ͽ��
        {
            return buf_to_data_ctl_on_off(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_act_8108(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x03:
        case 0x05:
        {
            return buf_to_data_cfg_8108_unit(pOadInfo,  convert);
        }break; 

        case 0x04:
        {
            return buf_to_data_comm_oi(pOadInfo,  convert);
        }break; 

        case 0x07:
        case 0x06:   //����Ͷ��ͽ��
        {
            return buf_to_data_ctl_on_off(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class13_act(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x8103:   //ʱ�ο�
        {
            return buf_to_data_act_8103(pOadInfo,  convert);
        }break;

        case 0x8104:   //���ݿ�
        {
            return buf_to_data_act_8104(pOadInfo,  convert);
        }break;

        case 0x8105:   //Ӫҵ��ͣ��
        {
            return buf_to_data_act_8105(pOadInfo,  convert);
        }break;  

        case 0x8106:   //�¸���
        {
            return buf_to_data_act_8106(pOadInfo,  convert);
        }break;  

        case 0x8107:   //�����
        {
            return buf_to_data_act_8107(pOadInfo,  convert);
        }break;  

        case 0x8108:   //�µ��
        {
            return buf_to_data_act_8108(pOadInfo,  convert);
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
* @name��       evt_rectable_get
* @brief��      ���¼���¼��
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E buf_to_data_set_8103(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return buf_to_data_cfg_8103(pOadInfo, convert);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��      buf_to_data_set_8104
* @brief��     ���ݿ�
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E buf_to_data_set_8104(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return buf_to_data_cfg_8104(pOadInfo, convert);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��      buf_to_data_set_8105
* @brief��     ���ݿ�
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E buf_to_data_set_8105(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return buf_to_data_cfg_8105(pOadInfo, convert);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��      buf_to_data_set_8106
* @brief��     ���ݿ�
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E buf_to_data_set_8106(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        //return buf_to_data_cfg_8106(pOadInfo, convert);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name��      buf_to_data_set_8107
* @brief��     ���ݿ�
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E buf_to_data_set_8107(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return buf_to_data_cfg_8107(pOadInfo, convert);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��      buf_to_data_set_8108
* @brief��     ���ݿ�
* @param[in] ��pOdaInfo        OAD��Ϣ
               pBpInfo        Ӧ�÷�֡�ϵ���Ϣ
               inData         ���뱨�ģ�����OAD
               inLen          ���뱨�ĳ���
               outDataMax     ������Ļ�������󳤶�

* @param[out]��outData     �������
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E buf_to_data_set_8108(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return buf_to_data_cfg_8108(pOadInfo, convert);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class13_set(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x8103:   //ʱ�οؿ��Ʒ�����
        {
            return buf_to_data_set_8103(pOadInfo,  convert);
        }break;

        case 0x8104:   //���ݿؿ��Ʒ�����
        {
            return buf_to_data_set_8104(pOadInfo,  convert);
        }break;

        case 0x8105:   //Ӫҵ��ͣ�ؿ��Ʒ�����
        {
            return buf_to_data_set_8105(pOadInfo,  convert);
        }break;  

        case 0x8106:   //�¸��ؿ��Ʒ�����
        {
            return buf_to_data_set_8106(pOadInfo,  convert);
        }break;  

        case 0x8107:   //����ؿ��Ʒ�����
        {
            return buf_to_data_set_8107(pOadInfo,  convert);
        }break;  

        case 0x8108:   //�µ�ؿ��Ʒ�����
        {
            return buf_to_data_set_8108(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_8103(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:   //ʱ�οؿ��Ʒ�����
        {
            return data_to_buf_cfg_8103(pOadInfo,  convert);
        }break;

        case 0x03:   //ʱ�οؿ���Ͷ��״̬
        {
            return data_to_buf_ctl_in_state(pOadInfo,  convert);
        }break;

        case 0x04:   //ʱ�οؿ������״̬
        {
            return data_to_buf_ctl_out_state(pOadInfo,  convert);
        }break;
        
        case 0x05:   //ʱ�οظ澯���״̬
        {
            return data_to_buf_ctl_alarm_state(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_8104(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:   //���ݿؿ��Ʒ�����
        {
            return data_to_buf_cfg_8104(pOadInfo,  convert);
        }break;

        case 0x03:   //ʱ�οؿ���Ͷ��״̬
        {
            return data_to_buf_ctl_in_state(pOadInfo,  convert);
        }break;

        case 0x04:   //ʱ�οؿ������״̬
        {
            return data_to_buf_ctl_out_state(pOadInfo,  convert);
        }break;
        
        case 0x05:   //ʱ�οظ澯���״̬
        {
            return data_to_buf_ctl_alarm_state(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_8105(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:   //Ӫҵ��ͣ�ؿ��Ʒ�����
        {
            return data_to_buf_cfg_8105(pOadInfo,  convert);
        }break;

        case 0x03:   //ʱ�οؿ���Ͷ��״̬
        {
            return data_to_buf_ctl_in_state(pOadInfo,  convert);
        }break;

        case 0x04:   //ʱ�οؿ������״̬
        {
            return data_to_buf_ctl_out_state(pOadInfo,  convert);
        }break;
        
        case 0x05:   //ʱ�οظ澯���״̬
        {
            return data_to_buf_ctl_alarm_state(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_8106(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        // case 0x02:   //�¸��ؿ��Ʒ�����
        // {
        //     return data_to_buf_cfg_8105(pOadInfo,  convert);
        // }break;

        case 0x03:   //�¸��ؿ���Ͷ��״̬
        {
            return data_to_buf_ctl_in_state(pOadInfo,  convert);
        }break;

        case 0x04:   //�¸��ؿ������״̬
        {
            return data_to_buf_ctl_out_state(pOadInfo,  convert);
        }break;
        
        case 0x05:   //�¸��ظ澯���״̬
        {
            return data_to_buf_ctl_alarm_state(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_8107(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:   //����ؿ��Ʒ�����
        {
            return data_to_buf_cfg_8107(pOadInfo,  convert);
        }break;
        
        case 0x03:   //ʱ�οؿ���Ͷ��״̬
        {
            return data_to_buf_ctl_in_state(pOadInfo,  convert);
        }break;

        case 0x04:   //ʱ�οؿ������״̬
        {
            return data_to_buf_ctl_out_state(pOadInfo,  convert);
        }break;
        
        case 0x05:   //ʱ�οظ澯���״̬
        {
            return data_to_buf_ctl_alarm_state(pOadInfo,  convert);
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
* @brief��     ��698��ʽ����ת�������ĵ�����(class22)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_get_8108(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.attID)
    {
        case 0x02:   //�µ�ؿ��Ʒ�����
        {
            return data_to_buf_cfg_8108(pOadInfo,  convert);
        }break;

        case 0x03:   //ʱ�οؿ���Ͷ��״̬
        {
            return data_to_buf_ctl_in_state(pOadInfo,  convert);
        }break;

        case 0x04:   //ʱ�οؿ������״̬
        {
            return data_to_buf_ctl_out_state(pOadInfo,  convert);
        }break;
        
        case 0x05:   //ʱ�οظ澯���״̬
        {
            return data_to_buf_ctl_alarm_state(pOadInfo,  convert);
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
* @brief��     ���������ĵ�����ת698��ʽ����(class13)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class13(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
        case 0x8103:   //ʱ�ο�
        {
            return data_to_buf_get_8103(pOadInfo,  convert);
        }break;
        
        case 0x8104:   //���ݿ�
        {
            return data_to_buf_get_8104(pOadInfo,  convert);
        }break;

        case 0x8105:   //Ӫҵ��ͣ��
        {
            return data_to_buf_get_8105(pOadInfo,  convert);
        }break;

        case 0x8106:   //�¸���
        {
            return data_to_buf_get_8106(pOadInfo,  convert);
        }break;

        case 0x8107:   //�����
        {
            return data_to_buf_get_8107(pOadInfo,  convert);
        }break;

        case 0x8108:   //�µ��
        {
            return data_to_buf_get_8108(pOadInfo,  convert);
        }break;
        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;
    }

    return DATA_SUCCESS;
}

OOP_DAR_E buf_to_data_class13(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8103_cfg_add(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81030300:
    case 0x81030500:
        {
            //action����
            uint32 i = 0;
            uint32 j = 0;
            OOP_CFGUNIT_8109_T dataSet;
            OOP_8103_CONFIG_T data;

            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%08x\n", i, dataSet.object);
            PRTL_FMT_LOGCC("dataSet.plan = %d\n", i, dataSet.plan);
            PRTL_FMT_LOGCC("dataSet.value[0].valid = %d\n", i, dataSet.value[0].valid);

            PRTL_FMT_LOGCC("dataSet.floatCoe = %d\n", i, dataSet.floatCoe);

            if ((dataSet.object > 0x2308) || (dataSet.object < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x81030200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            i = dataSet.object - 0x2301;
            data.item[i] = dataSet;

            if (data.nNum < i + 1)
            {
                data.nNum = i + 1;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].plan = %d\n", i, data.item[i].plan);
                PRTL_FMT_LOGCC("data.item[%d].value[0].valid = %d\n", i, data.item[i].value[0].valid);
                for (j = 0; j < OOP_MAX_TURN; j++)
                {
                    PRTL_FMT_LOGCC("data.item[%d].value[0].defValue[j] = %lld\n", i, data.item[i].value[0].defValue[j]);
                }

                // .....ʡ������2��

                PRTL_FMT_LOGCC("data.item[%d].floatCoe = %d\n", i, data.item[i].floatCoe);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81030200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8103_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8103_cfg_del(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81030400:
        {
            //action����
            uint32 i = 0;
            uint32 j = 0;
            OOP_CFGUNIT_8109_T dataSet;
            OOP_8103_CONFIG_T data;

            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%08x\n", i, dataSet.object);
            PRTL_FMT_LOGCC("dataSet.plan = %d\n", i, dataSet.plan);
            PRTL_FMT_LOGCC("dataSet.value[0].valid = %d\n", i, dataSet.value[0].valid);

            PRTL_FMT_LOGCC("dataSet.floatCoe = %d\n", i, dataSet.floatCoe);

            if ((dataSet.object > 0x2308) || (dataSet.object < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x81030200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            i = dataSet.object - 0x2301;
            if (i >= OOP_MAX_TG)
            {
                PRTL_FMT_LOGCC("dataSet.object err\n");
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            MEMZERO(&data.item[i], sizeof(data.item[i]));

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].plan = %d\n", i, data.item[i].plan);
                PRTL_FMT_LOGCC("data.item[%d].value[0].valid = %d\n", i, data.item[i].value[0].valid);
                for (j = 0; j < OOP_MAX_TURN; j++)
                {
                    PRTL_FMT_LOGCC("data.item[%d].value[0].defValue[j] = %lld\n", i, data.item[i].value[0].defValue[j]);
                }

                // .....ʡ������2��

                PRTL_FMT_LOGCC("data.item[%d].floatCoe = %d\n", i, data.item[i].floatCoe);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81030200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8103_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8103_on(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81030600:
        {
            //action����
            uint8 dataSet = 1;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8103_on oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x810306FF;
            normal.infoNum = oi - 0x2300;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8103_off(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81030700:
        {
            //action����
            uint8 dataSet = 0;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8103_off oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810306FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    sleep(2);
    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8103_switch(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81037F00:
        {
            //action����
            OOP_POWCON_PERIOD_T dataSet;
            OOP_8103_CONFIG_T   tConfig;
            uint32              configLen = 0;
            uint8               i;
            OI oi = 0;

            memcpy(&dataSet, convert.dstBuf, dOffset);
            oi = dataSet.object;
            
            PRTL_FMT_TRACE("class13_invoke_act_8103_switch oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301) || dataSet.plan.planID > 2)
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.oad.value = 0x81030200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_8103_CONFIG_T), (uint8*)&tConfig, &configLen);
            if(0 != ret)
            {
                return DATA_READ_WRITE_DENIED;
            }
            /* �޸����� */
            for(i = 0; i < tConfig.nNum; i++)
            {
                if(tConfig.item[i].object == dataSet.object && dataSet.plan.planID < 3)
                {
                    tConfig.item[i].value[0].valid = 0;
                    tConfig.item[i].value[1].valid = 0;
                    tConfig.item[i].value[2].valid = 0;
                    tConfig.item[i].value[dataSet.plan.planID].valid = dataSet.plan.flag.nValue;
                    break;
                }
            }
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&tConfig, sizeof(OOP_8103_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }

            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x81037FFF;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_TRACE("class13_invoke_act_8103_switch dataSet.object = 0x%04x\n", dataSet.object);
            PRTL_FMT_TRACE("class13_invoke_act_8103_switch dataSet.plan.flag = 0x%02x\n", dataSet.plan.flag.nValue);
            PRTL_FMT_TRACE("class13_invoke_act_8103_switch dataSet.plan.planID = %d\n", dataSet.plan.planID);
            
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_POWCON_PERIOD_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8104_cfg_add(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81040300:
    case 0x81040500:
        {
            //action����
            uint32 i = 0;
            OOP_CFGUNIT_810A_T dataSet;
            OOP_8104_CONFIG_T data;

            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%08x\n", dataSet.object);
            PRTL_FMT_LOGCC("dataSet.defValue = %lld\n", dataSet.defValue);
            PRTL_FMT_LOGCC("dataSet.value[0].startTime.year = %d\n", dataSet.startTime.year);
            PRTL_FMT_LOGCC("dataSet.duration = %d\n", dataSet.duration);
            PRTL_FMT_LOGCC("dataSet.powLimit = 0x%x\n", dataSet.powLimit);

            if ((dataSet.object > 0x2308) || (dataSet.object < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x81040200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            i = dataSet.object - 0x2301;
            data.item[i] = dataSet;

            if (data.nNum < i + 1)
            {
                data.nNum = i + 1;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].defValue = %lld\n", i, data.item[i].defValue);
                PRTL_FMT_LOGCC("data.item[%d].startTime.year = %d\n", i, data.item[i].startTime.year);
                PRTL_FMT_LOGCC("data.item[%d].duration = %d\n", i, data.item[i].duration);
                PRTL_FMT_LOGCC("data.item[%d].powLimit = 0x%x\n", i, data.item[i].powLimit);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81040200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8104_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8104_cfg_del(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81040400:
        {
            //action����
            uint32 i = 0;
            OOP_CFGUNIT_810A_T dataSet;
            OOP_8104_CONFIG_T data;

            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%08x\n", dataSet.object);
            PRTL_FMT_LOGCC("dataSet.defValue = %lld\n", dataSet.defValue);
            PRTL_FMT_LOGCC("dataSet.value[0].startTime.year = %d\n", dataSet.startTime.year);
            PRTL_FMT_LOGCC("dataSet.duration = %d\n", dataSet.duration);
            PRTL_FMT_LOGCC("dataSet.powLimit = 0x%x\n", dataSet.powLimit);

            //����
            if ((dataSet.object > 0x2308) || (dataSet.object < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x81040200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            i = dataSet.object - 0x2301;
            if (i >= OOP_MAX_TG)
            {
                PRTL_FMT_LOGCC("dataSet.object err\n");
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            MEMZERO(&data.item[i], sizeof(data.item[i]));

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].defValue = %lld\n", i, data.item[i].defValue);
                PRTL_FMT_LOGCC("data.item[%d].startTime.year = %d\n", i, data.item[i].startTime.year);
                PRTL_FMT_LOGCC("data.item[%d].duration = %d\n", i, data.item[i].duration);
                PRTL_FMT_LOGCC("data.item[%d].powLimit = 0x%x\n", i, data.item[i].powLimit);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81040200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8104_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8104_on(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81040600:
        {
            //action����
            uint8 dataSet = 1;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8104_on oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810406FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8104_off(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81040700:
        {
            //action����
            uint8 dataSet = 0;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8104_off oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810406FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    sleep(2);
    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8105_cfg_add(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81050300:
    case 0x81050500:
        {
            //action����
            uint32 i = 0;
            OOP_CFGUNIT_810B_T dataSet;
            OOP_8105_CONFIG_T data;

            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%08x\n", dataSet.object);
            PRTL_FMT_LOGCC("dataSet.startTime.year = %d\n", dataSet.startTime.year);
            PRTL_FMT_LOGCC("dataSet.endTime.year = %d\n", dataSet.endTime.year);
            PRTL_FMT_LOGCC("dataSet.defValue = %lld\n", dataSet.defValue);

            if ((dataSet.object > 0x2308) || (dataSet.object < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x81050200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            i = dataSet.object - 0x2301;
            data.item[i] = dataSet;

            if (data.nNum < i + 1)
            {
                data.nNum = i + 1;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].startTime.year = %d\n", i, data.item[i].startTime.year);
                PRTL_FMT_LOGCC("data.item[%d].endTime.year = %d\n", i, data.item[i].endTime.year);
                PRTL_FMT_LOGCC("data.item[%d].defValue = %lld\n", i, data.item[i].defValue);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81050200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8105_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8105_cfg_del(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81050400:
        {
            //action����
            uint32 i = 0;
            OOP_CFGUNIT_810B_T dataSet;
            OOP_8105_CONFIG_T data;

            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%08x\n", dataSet.object);
            PRTL_FMT_LOGCC("dataSet.startTime.year = %d\n", dataSet.startTime.year);
            PRTL_FMT_LOGCC("dataSet.endTime.year = %d\n", dataSet.endTime.year);
            PRTL_FMT_LOGCC("dataSet.defValue = %lld\n", dataSet.defValue);

            if ((dataSet.object > 0x2308) || (dataSet.object < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x81030200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            i = dataSet.object - 0x2301;
            if (i >= OOP_MAX_TG)
            {
                PRTL_FMT_LOGCC("dataSet.object err\n");
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            MEMZERO(&data.item[i], sizeof(data.item[i]));

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].startTime.year = %d\n", i, data.item[i].startTime.year);
                PRTL_FMT_LOGCC("data.item[%d].endTime.year = %d\n", i, data.item[i].endTime.year);
                PRTL_FMT_LOGCC("data.item[%d].defValue = %lld\n", i, data.item[i].defValue);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81050200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8105_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8105_on(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81050600:
        {
            //action����
            uint8 dataSet = 1;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8105_on oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810506FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8105_off(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81050700:
        {
            //action����
            uint8 dataSet = 0;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8105_off oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810506FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    sleep(2);
    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8106_on(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    //NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81060600:
        {
            //action����
            // uint8 dataSet = 1;
            // //memcpy(&dataSet, convert.dstBuf, dOffset);
            // normal.oad.value = 0x810606FF;
            // normal.classtag = CLASS_DATA_INIT;

            // ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            // if(ret != ERR_OK)
            // {
            //     PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            //         ret, normal.logicId, normal.infoNum, normal.oad.value);
            //     return DATA_OTHER_REASON;
            // }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8106_off(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81060700:
        {
            //action����
            uint8 dataSet = 0;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8106_off oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810606FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    sleep(2);
    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8106_powcon_low(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81067f00:
    case 0x8106d900:
        {
            //action����
            uint8 dataAct = 1;
            OOP_POWCON_LOW_T dataSet;
            OI oi = 0;

            memcpy(&dataSet, convert.dstBuf, dOffset);
            oi = dataSet.object;
            PRTL_FMT_TRACE("class13_invoke_act_8106_powcon_low oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x81067fff;
            normal.classtag = CLASS_DATA_INIT;

            PRTL_FMT_LOGCC("dataSet.scheme.object = 0x%x\n", dataSet.object);
            PRTL_FMT_LOGCC("dataSet.scheme.plan = %d\n", dataSet.scheme.slipTime);
            PRTL_FMT_LOGCC("dataSet.scheme.floatCoefficient = %d\n", dataSet.scheme.floatCoefficient);
            PRTL_FMT_LOGCC("dataSet.scheme.delayTime = %d\n", dataSet.scheme.delayTime);
            PRTL_FMT_LOGCC("dataSet.scheme.conTime = %d\n", dataSet.scheme.conTime);
            PRTL_FMT_LOGCC("dataSet.scheme.delayTime[0] = %d\n", dataSet.scheme.waringTime[0]);
            PRTL_FMT_LOGCC("dataSet.scheme.delayTime[1] = %d\n", dataSet.scheme.waringTime[1]);
            PRTL_FMT_LOGCC("dataSet.scheme.delayTime[2] = %d\n", dataSet.scheme.waringTime[2]);
            PRTL_FMT_LOGCC("dataSet.scheme.delayTime[3] = %d\n", dataSet.scheme.waringTime[3]);


            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_POWCON_LOW_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }

            memset(&normal, 0, sizeof(normal));
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810606FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataAct, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8107_cfg_add(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81070300:
    case 0x81070500:
        {
            //action����
            uint32 i = 0;
            OOP_CFGUNIT_810C_T dataSet;
            OOP_CFGUNIT_810C_T latest810Cdata;
            OOP_8107_CONFIG_T data;
            int64 remainEN = 0;
            int64 newEN = 0;
            OOP_TGSETS_T tGroup;


            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%04x\n", dataSet.object);
            PRTL_FMT_LOGCC("dataSet.number = %d\n", dataSet.number);
            PRTL_FMT_LOGCC("dataSet.flag = %d\n", dataSet.flag);
            PRTL_FMT_LOGCC("dataSet.type = %d\n", dataSet.type);
            PRTL_FMT_LOGCC("dataSet.purElectricity = %lld\n", dataSet.purElectricity);
            PRTL_FMT_LOGCC("dataSet.alarmLimit = %lld\n", dataSet.alarmLimit);
            PRTL_FMT_LOGCC("dataSet.tripLimit = %lld\n", dataSet.tripLimit);
            PRTL_FMT_LOGCC("dataSet.mode = %d\n", dataSet.mode);

            //���ݴ���
            if ((dataSet.object > 0x2308) || 
                (dataSet.object < 0x2301) ||
                (dataSet.flag > 1)        ||
                (dataSet.type > 1)        ||
                (dataSet.mode > 1))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = dataSet.object * 0x00010000 + 0x0200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_TGSETS_T), (uint8*)&tGroup, &len);
            if ((ret != ERR_OK) || (len != sizeof(OOP_TGSETS_T)))
            {
                PRTL_FMT_DEBUG("�ܼ���%dû������\n", dataSet.object - 0x2300);
                return DATA_OTHER_REASON;
            }
            if(0 == tGroup.nNum)
            {
                PRTL_FMT_DEBUG("�ܼ���%dû������\n", dataSet.object - 0x2300);
                return DATA_OTHER_REASON;
            }
                
            normal.oad.value = 0x81070200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_8107_CONFIG_T), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len != sizeof(OOP_8107_CONFIG_T)))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x) len(%d)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value, len);
            }

            i = dataSet.object - 0x2301;
            if ((data.item[i].object > 0x2308) || 
                (data.item[i].object < 0x2301) ||
                (data.item[i].flag > 1)        ||
                (data.item[i].type > 1)        ||
                (data.item[i].mode > 1))
            {
                PRTL_FMT_TRACE("OI[0x%04x] inexistence\n", dataSet.object);
            }
            else
            {
                PRTL_FMT_TRACE("OI[0x%04x] exist\n", dataSet.object);

                //׷�ӷ�ʽʱ�����ι��絥�ź��ϴ���ͬ��ֱ�ӷ���
                if ((dataSet.flag == 0) && (data.item[i].number == dataSet.number))
                {
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }

                //׷�ӷ�ʽ����Ҫ�жϹ��������Ƿ�һ��
                if ((dataSet.flag == 0) && (data.item[i].type != dataSet.type))
                {
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }

                //ˢ�·�ʽʱ�����ι��絥�ź��ϴ���ͬ�򷵻سɹ�
                if ((dataSet.flag == 1) && (data.item[i].number == dataSet.number))
                {
                    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
                    offset += sizeof(OOP_OAD_U);
                    outData->pResultData[offset ++] = 0x00;                            //DAR type
                    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
                    outData->resultDataLen = offset;
                    return dar;
                }
            }

            //ʣ���������
            remain_energy_lock();
            dar = remain_energy_exec(pApduInfo->hUdp, &remainEN, dataSet, &newEN);
            if (dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("remain_energy_exec err dar[%d]\n", dar);
                remain_energy_unlock();
                return dar;
            }
            remain_energy_unlock();

            latest810Cdata = data.item[i];
            data.item[i] = dataSet;

            if (data.nNum < i + 1)
            {
                data.nNum = i + 1;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%04x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].number = %d\n", i, data.item[i].number);
                PRTL_FMT_LOGCC("data.item[%d].flag = %d\n", i, data.item[i].flag);
                PRTL_FMT_LOGCC("data.item[%d].type = %d\n", i, data.item[i].type);
                PRTL_FMT_LOGCC("data.item[%d].purElectricity = %lld\n", i, data.item[i].purElectricity);
                PRTL_FMT_LOGCC("data.item[%d].alarmLimit = %lld\n", i, data.item[i].alarmLimit);
                PRTL_FMT_LOGCC("data.item[%d].tripLimit = %lld\n", i, data.item[i].tripLimit);
                PRTL_FMT_LOGCC("data.item[%d].mode = %d\n", i, data.item[i].mode);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81070200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8107_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);

                //���絥�ű���ʧ�ܣ���Ҫ�ָ�ʣ����
                remain_energy_lock();
                if (remain_energy_write(pApduInfo->hUdp, i + 1, remainEN) != ERR_OK)
                {
                    PRTL_FMT_DEBUG("remain_energy_write err\n");
                }
                remain_energy_unlock();

                return DATA_OTHER_REASON;
            }

            /* ����سɹ� ��¼һ�¹���ǰ��ʣ����� */
            normal.oad.value = 0x810703ff;  //˽��OAD
            normal.classtag = CLASS_DATA_INIT;
            normal.infoNum = dataSet.object - 0x2300;
            db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&remainEN, sizeof(int64));
            normal.oad.value = 0x810704ff;  //˽��OAD
            db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&newEN, sizeof(int64));
            normal.oad.value = 0x810705ff;  //˽��OAD
            db_write_nomal(pApduInfo->hUdp, &normal, &latest810Cdata.type, sizeof(uint8));

            /* �����¼� */
            CheckERC_3202(pApduInfo->hUdp, dataSet.object, latest810Cdata);
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8107_cfg_del(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81070400:
        {
            //action����
            uint32 i = 0;
            OOP_CFGUNIT_810C_T dataSet;
            OOP_8107_CONFIG_T data;

            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%08x\n", i, dataSet.object);
            PRTL_FMT_LOGCC("dataSet.number = %d\n", i, dataSet.number);
            PRTL_FMT_LOGCC("dataSet.flag = %d\n", i, dataSet.flag);
            PRTL_FMT_LOGCC("dataSet.type = %d\n", i, dataSet.type);
            PRTL_FMT_LOGCC("dataSet.purElectricity = %lld\n", i, dataSet.purElectricity);
            PRTL_FMT_LOGCC("dataSet.alarmLimit = %lld\n", i, dataSet.alarmLimit);
            PRTL_FMT_LOGCC("dataSet.tripLimit = %lld\n", i, dataSet.tripLimit);
            PRTL_FMT_LOGCC("dataSet.mode = %d\n", i, dataSet.mode);

            if ((dataSet.object > 0x2308) || (dataSet.object < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x81070200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            i = dataSet.object - 0x2301;
            if (i >= OOP_MAX_TG)
            {
                PRTL_FMT_LOGCC("dataSet.object err\n");
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            MEMZERO(&data.item[i], sizeof(data.item[i]));

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].number = %d\n", i, data.item[i].number);
                PRTL_FMT_LOGCC("data.item[%d].flag = %d\n", i, data.item[i].flag);
                PRTL_FMT_LOGCC("data.item[%d].type = %d\n", i, data.item[i].type);
                PRTL_FMT_LOGCC("data.item[%d].purElectricity = %lld\n", i, data.item[i].purElectricity);
                PRTL_FMT_LOGCC("data.item[%d].alarmLimit = %lld\n", i, data.item[i].alarmLimit);
                PRTL_FMT_LOGCC("data.item[%d].tripLimit = %lld\n", i, data.item[i].tripLimit);
                PRTL_FMT_LOGCC("data.item[%d].mode = %d\n", i, data.item[i].mode);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81070200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8107_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8107_on(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81070600:
        {
            //action����
            uint8 dataSet = 1;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8107_on oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810706FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8107_off(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81070700:
        {
            //action����
            uint8 dataSet = 0;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8107_off oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810706FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    sleep(2);
    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8108_cfg_add(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81080300:
    case 0x81080500:
        {
            //action����
            uint32 i = 0;
            OOP_CFGUNIT_810D_T dataSet;
            OOP_8108_CONFIG_T data;

            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%08x\n", dataSet.object);
            PRTL_FMT_LOGCC("dataSet.defValue = %lld\n", dataSet.defValue);
            PRTL_FMT_LOGCC("dataSet.limitCoe = %d\n", dataSet.limitCoe);
            PRTL_FMT_LOGCC("dataSet.floatCoe = %d\n", dataSet.floatCoe);

            if ((dataSet.object > 0x2308) || (dataSet.object < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x81080200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            i = dataSet.object - 0x2301;
            data.item[i] = dataSet;

            if (data.nNum < i + 1)
            {
                data.nNum = i + 1;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].defValue = %lld\n", i, data.item[i].defValue);
                PRTL_FMT_LOGCC("data.item[%d].limitCoe = %d\n", i, data.item[i].limitCoe);
                PRTL_FMT_LOGCC("data.item[%d].floatCoe = %d\n", i, data.item[i].floatCoe);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81080200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8108_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8108_cfg_del(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_act failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81080400:
        {
            //action����
            uint32 i = 0;
            OOP_CFGUNIT_810D_T dataSet;
            OOP_8108_CONFIG_T data;

            MEMZERO(&dataSet, sizeof(dataSet));
            MEMZERO(&data, sizeof(data));

            memcpy(&dataSet, convert.dstBuf, dOffset);
            PRTL_FMT_LOGCC("dataSet.object = 0x%08x\n", dataSet.object);
            PRTL_FMT_LOGCC("dataSet.defValue = %lld\n", dataSet.defValue);
            PRTL_FMT_LOGCC("dataSet.limitCoe = %d\n", dataSet.limitCoe);
            PRTL_FMT_LOGCC("dataSet.floatCoe = %d\n", dataSet.floatCoe);

            if ((dataSet.object > 0x2308) || (dataSet.object < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }

            normal.oad.value = 0x81080200;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
            }

            i = dataSet.object - 0x2301;
            if (i >= OOP_MAX_TG)
            {
                PRTL_FMT_LOGCC("dataSet.object err\n");
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            MEMZERO(&data.item[i], sizeof(data.item[i]));

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].defValue = %lld\n", i, data.item[i].defValue);
                PRTL_FMT_LOGCC("data.item[%d].limitCoe = %d\n", i, data.item[i].limitCoe);
                PRTL_FMT_LOGCC("data.item[%d].floatCoe = %d\n", i, data.item[i].floatCoe);
            }

            MEMZERO(&normal, sizeof(normal));
            normal.oad.value = 0x81080200;
            normal.classtag = CLASS_DATA_INIT;
            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_8108_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8108_on(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81080600:
        {
            //action����
            uint8 dataSet = 1;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8108_on oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810806FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_act_8108_off(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    NOMAL_OAD_T normal = {0};
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_act(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81080700:
        {
            //action����
            uint8 dataSet = 0;
            OI oi = 0;
            memcpy(&oi, convert.dstBuf, dOffset);

            PRTL_FMT_TRACE("class13_invoke_act_8108_off oi[0x%04x]\n", oi);
            if ((oi > 0x2308) || (oi < 0x2301))
            {
                return DATA_SCOPE_OF_ACCESS_VIOLATED;
            }
            
            normal.infoNum = oi - 0x2300;
            normal.oad.value = 0x810806FF;
            normal.classtag = CLASS_DATA_INIT;

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(uint8));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    sleep(2);
    return dar;
}

/**
*********************************************************************
* @name��       class13_invoke_act
* @brief��      class13��ACTION����
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
OOP_DAR_E class13_invoke_act_8103(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OMD_U omd = {0};

    memcpy(&omd, &pOadInfo->pTab->oad, sizeof(uint32));

    switch (omd.nModID)
    {
    case 0x03:
    case 0x05:
        return class13_invoke_act_8103_cfg_add(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_act_8103_cfg_del(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x06:
        return class13_invoke_act_8103_on(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0x07:
        return class13_invoke_act_8103_off(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x7f:
        return class13_invoke_act_8103_switch(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_act
* @brief��      class13��ACTION����
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
OOP_DAR_E class13_invoke_act_8104(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x03:
    case 0x05:
        return class13_invoke_act_8104_cfg_add(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_act_8104_cfg_del(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x06:
        return class13_invoke_act_8104_on(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0x07:
        return class13_invoke_act_8104_off(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_act
* @brief��      class13��ACTION����
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
OOP_DAR_E class13_invoke_act_8105(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x03:
    case 0x05:
        return class13_invoke_act_8105_cfg_add(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_act_8105_cfg_del(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x06:
        return class13_invoke_act_8105_on(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0x07:
        return class13_invoke_act_8105_off(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_act
* @brief��      class13��ACTION����
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
OOP_DAR_E class13_invoke_act_8106(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OMD_U omd = {0};

    memcpy(&omd, &pOadInfo->pTab->oad, sizeof(uint32));

    switch (omd.nModID)
    {
    case 0x06:
        return class13_invoke_act_8106_on(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0x07:
        return class13_invoke_act_8106_off(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x7F:
	case 0xD9:
        return class13_invoke_act_8106_powcon_low(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_act
* @brief��      class13��ACTION����
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
OOP_DAR_E class13_invoke_act_8107(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x03:
    case 0x05:
        return class13_invoke_act_8107_cfg_add(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_act_8107_cfg_del(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x06:
        return class13_invoke_act_8107_on(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0x07:
        return class13_invoke_act_8107_off(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_act
* @brief��      class13��ACTION����
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
OOP_DAR_E class13_invoke_act_8108(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x03:
    case 0x05:
        return class13_invoke_act_8108_cfg_add(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_act_8108_cfg_del(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x06:
        return class13_invoke_act_8108_on(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0x07:
        return class13_invoke_act_8108_off(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_set_ctl_8103(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81030200:
        {
            uint32 i = 0;
            OOP_8103_CONFIG_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            for (i = 0; i < dataSet.nNum; i++)
            {
                //����
                if ((dataSet.item[i].object > 0x2308) || (dataSet.item[i].object < 0x2301))
                {
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }
                PRTL_FMT_LOGCC("dataSet.item[%d].object = 0x%08x\n", i, dataSet.item[i].object);
                PRTL_FMT_LOGCC("dataSet.item[%d].plan = %d\n", i, dataSet.item[i].plan);
                PRTL_FMT_LOGCC("dataSet.item[%d].value[0].valid = %d\n", i, dataSet.item[i].value[0].valid);

                PRTL_FMT_LOGCC("dataSet.item[%d].floatCoe = %d\n", i, dataSet.item[i].floatCoe);
            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_8103_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_set_ctl_8104(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81040200:
        {
            uint32 i = 0;
            OOP_8104_CONFIG_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            for (i = 0; i < dataSet.nNum; i++)
            {
                PRTL_FMT_LOGCC("dataSet.item[%d].object = 0x%08x\n", i, dataSet.item[i].object);
                PRTL_FMT_LOGCC("dataSet.item[%d].defValue = %lld\n", i, dataSet.item[i].defValue);
                PRTL_FMT_LOGCC("dataSet.item[%d].value[0].startTime.year = %d\n", i, dataSet.item[i].startTime.year);
                PRTL_FMT_LOGCC("dataSet.item[%d].duration = %d\n", i, dataSet.item[i].duration);
                PRTL_FMT_LOGCC("dataSet.item[%d].powLimit = 0x%x\n", i, dataSet.item[i].powLimit);
                //����
                if ((dataSet.item[i].object > 0x2308) || (dataSet.item[i].object < 0x2301))
                {
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }

            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_8104_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_set_ctl_8105(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81050200:
        {
            uint32 i = 0;
            OOP_8105_CONFIG_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            for (i = 0; i < dataSet.nNum; i++)
            {
                PRTL_FMT_LOGCC("dataSet.item[%d].object = 0x%08x\n", i, dataSet.item[i].object);
                PRTL_FMT_LOGCC("dataSet.item[%d].startTime.year = %d\n", i, dataSet.item[i].startTime.year);
                PRTL_FMT_LOGCC("dataSet.item[%d].endTime.year = %d\n", i, dataSet.item[i].endTime.year);
                PRTL_FMT_LOGCC("dataSet.item[%d].defValue = %lld\n", i, dataSet.item[i].defValue);
                 //����
                if ((dataSet.item[i].object > 0x2308) || (dataSet.item[i].object < 0x2301))
                {
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }
            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_8105_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_set_ctl_8107(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81070200:
        {
            uint32 i = 0;
            uint32 j = 0;
            OOP_8107_CONFIG_T dataSet;
            OOP_8107_CONFIG_T dataGet;
            OOP_8107_CONFIG_T dataNew;
            OOP_CFGUNIT_810C_T latest810Cdata;
            uint8              buytype = 0;
            OOP_TGSETS_T       tGroup;
            int64 remainEN[OOP_MAX_TG] = {0};
            int64 newEN[OOP_MAX_TG] = {0};
            memcpy(&dataSet, convert.dstBuf, dOffset);

            /* �ܼ��� */
            for(i = 0; i < dataSet.nNum; i++)
            {
                if (dataSet.item[i].object > 0x2308 || dataSet.item[i].object < 0x2301)
                {
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }

                normal.oad.value = dataSet.item[i].object * 0x00010000 + 0x0200;
                ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_TGSETS_T), (uint8*)&tGroup, &len);
                if ((ret != ERR_OK) || (len != sizeof(OOP_TGSETS_T)))
                {
                    PRTL_FMT_DEBUG("�ܼ���%dû������\n", dataSet.item[i].object - 0x2300);
                    return DATA_OTHER_REASON;
                }
                if(0 == tGroup.nNum)
                {
                    PRTL_FMT_DEBUG("�ܼ���%dû������\n", dataSet.item[i].object - 0x2300);
                    return DATA_OTHER_REASON;
                }
            }
            
            normal.oad.value = 0x81070200;
            memset(&dataGet, 0, sizeof(dataGet));
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&dataGet, &len);
            if ((ret != ERR_OK) || (len != pOadInfo->pTab->dataLen))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x) len(%d)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value, len);
            }

            memcpy(&dataNew, &dataGet, sizeof(OOP_8107_CONFIG_T));
            dataNew.nNum = OOP_MAX_TG;
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;
            for (i = 0; i < dataSet.nNum; i++)
            {
                PRTL_FMT_LOGCC("dataSet.item[%d].object = 0x%08x\n", i, dataSet.item[i].object);
                PRTL_FMT_LOGCC("dataSet.item[%d].number = %d\n", i, dataSet.item[i].number);
                PRTL_FMT_LOGCC("dataSet.item[%d].flag = %d\n", i, dataSet.item[i].flag);
                PRTL_FMT_LOGCC("dataSet.item[%d].type = %d\n", i, dataSet.item[i].type);
                PRTL_FMT_LOGCC("dataSet.item[%d].purElectricity = %lld\n", i, dataSet.item[i].purElectricity);
                PRTL_FMT_LOGCC("dataSet.item[%d].alarmLimit = %lld\n", i, dataSet.item[i].alarmLimit);
                PRTL_FMT_LOGCC("dataSet.item[%d].tripLimit = %lld\n", i, dataSet.item[i].tripLimit);
                PRTL_FMT_LOGCC("dataSet.item[%d].mode = %d\n", i, dataSet.item[i].mode);
                
                j = dataSet.item[i].object - 0x2301;
                dataNew.item[j] = dataSet.item[i];
                
                if ((dataGet.item[j].object > 0x2308) || 
                    (dataGet.item[j].object < 0x2301) ||
                    (dataGet.item[j].flag > 1)        ||
                    (dataGet.item[j].type > 1)        ||
                    (dataGet.item[j].mode > 1))
                {                    
                    PRTL_FMT_TRACE("OI[0x%04x] inexistence\n", dataGet.item[j].object);
                }
                else
                {
                    PRTL_FMT_TRACE("OI[0x%04x] exist\n", dataSet.item[i].object);

                    //׷�ӷ�ʽʱ�����ι��絥�ź��ϴ���ͬ��ֱ�ӷ���
                    if((dataSet.item[i].flag == 0) && (dataSet.item[i].number == dataGet.item[j].number))
                    {
                        return DATA_SCOPE_OF_ACCESS_VIOLATED;
                    }

                    //׷�ӷ�ʽ����Ҫ�жϹ��������Ƿ�һ��
                    if((dataSet.item[i].flag == 0) && (dataSet.item[i].type != dataGet.item[j].type))
                    {
                        return DATA_SCOPE_OF_ACCESS_VIOLATED;
                    }
                    
                    //ˢ�·�ʽʱ�����ι��絥�ź��ϴ���ͬ��ֱ�ӷ��سɹ�
                    if((dataSet.item[i].flag == 1) && (dataSet.item[i].number == dataGet.item[j].number))
                    {
                        memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
                        offset += sizeof(OOP_OAD_U);
                        outData->pResultData[offset ++] = 0x00;                            //DAR type
                        outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
                        outData->resultDataLen = offset;
                        return dar;
                    }
                }

                //ʣ���������
                remain_energy_lock();
                dar = remain_energy_exec(pApduInfo->hUdp, &remainEN[i], dataSet.item[i], &newEN[i]);
                if (dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("remain_energy_exec err dar[%d]\n", dar);
                    remain_energy_unlock();
                    return dar;
                }
                remain_energy_unlock();
            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataNew, sizeof(OOP_8107_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);

                //���絥�ű���ʧ�ܣ���Ҫ�ָ�ʣ����
                for (i = 0; i < dataSet.nNum; i++)
                {
                    remain_energy_lock();
                    if (remain_energy_write(pApduInfo->hUdp, dataSet.item[i].object - 0x2300, remainEN[i]) != ERR_OK)
                    {
                        PRTL_FMT_DEBUG("remain_energy_write err\n");
                    }
                    remain_energy_unlock();
                }

                return DATA_OTHER_REASON;
            }
            else
            {
                for (i = 0; i < dataSet.nNum; i++)
                {
                    j = dataSet.item[i].object - 0x2301;
                    latest810Cdata = dataGet.item[j];
                    
                    buytype = dataGet.item[j].type;

                    /* ����سɹ� ��¼һ�¹���ǰ��ʣ����� */
                    normal.oad.value = 0x810703ff;  //˽��OAD
                    normal.classtag = CLASS_DATA_INIT;
                    normal.infoNum = dataSet.item[i].object - 0x2300;
                    db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&remainEN[i], sizeof(int64));
                    normal.oad.value = 0x810704ff;  //˽��OAD
                    db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&newEN[i], sizeof(int64));
                    normal.oad.value = 0x810705ff;  //˽��OAD
                    db_write_nomal(pApduInfo->hUdp, &normal, &buytype, sizeof(uint8));

                    /* �����¼� */
                    CheckERC_3202(pApduInfo->hUdp, dataSet.item[i].object, latest810Cdata);
                }
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��       class22_invoke_act
* @brief��      class22��ACTION����
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
OOP_DAR_E class13_invoke_set_ctl_8108(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data[BUF_LEN_MAX] = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    uint32 offset  = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (uint8 *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_class13_set(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_class13_set failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //д������
    switch (pOadInfo->pTab->oad.value)
    {
    case 0x81080200:
        {
            uint32 i = 0;
            OOP_8108_CONFIG_T dataSet;
            memcpy(&dataSet, convert.dstBuf, dOffset);
            normal.oad.value = pOadInfo->pTab->oad.value;
            normal.classtag = CLASS_DATA_INIT;

            for (i = 0; i < dataSet.nNum; i++)
            {
                 //����
                if ((dataSet.item[i].object > 0x2308) || (dataSet.item[i].object < 0x2301))
                {
                    return DATA_SCOPE_OF_ACCESS_VIOLATED;
                }
                PRTL_FMT_LOGCC("dataSet.item[%d].object = 0x%08x\n", i, dataSet.item[i].object);
                PRTL_FMT_LOGCC("dataSet.item[%d].defValue = %lld\n", i, dataSet.item[i].defValue);
                PRTL_FMT_LOGCC("dataSet.item[%d].limitCoe = %d\n", i, dataSet.item[i].limitCoe);
                PRTL_FMT_LOGCC("dataSet.item[%d].floatCoe = %d\n", i, dataSet.item[i].floatCoe);
            }

            ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&dataSet, sizeof(OOP_8108_CONFIG_T));
            if(ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                return DATA_OTHER_REASON;
            }
        }
        break;
    
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x00;                            //DAR type
    outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_ctl_in_state(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x03:
        {
            uint32 i = 0;
            OOP_TGSTATES_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].obj = 0x%08x\n", i, data.item[i].obj);
                PRTL_FMT_LOGCC("data.item[%d].state = %d\n", i, data.item[i].state);
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
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class13(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class13 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_ctl_out_state(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x04:
        {
            uint32 i = 0;
            OOP_TURNSTATES_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].obj = 0x%08x\n", i, data.item[i].obj);
                PRTL_FMT_LOGCC("data.item[%d].state = %d\n", i, data.item[i].state);
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
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class13(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class13 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_ctl_alarm_state(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x05:
        {
            uint32 i = 0;
            OOP_TGSTATES_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].obj = 0x%08x\n", i, data.item[i].obj);
                PRTL_FMT_LOGCC("data.item[%d].state = %d\n", i, data.item[i].state);
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
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
    
    /* ���ݺͱ���ת�� */
    dar = data_to_buf_class13(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class13 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_cfg_8103(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        {
            uint32 i = 0;
            uint32 j = 0;
            OOP_8103_CONFIG_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].plan = %d\n", i, data.item[i].plan);
                PRTL_FMT_LOGCC("data.item[%d].value[0].valid = %d\n", i, data.item[i].value[0].valid);

                for (j = 0; j < OOP_MAX_TURN; j++)
                {
                    PRTL_FMT_LOGCC("data.item[%d].value[0].defValue[j] = %lld\n", i, data.item[i].value[0].defValue[j]);
                }

                // .....ʡ������2��

                PRTL_FMT_LOGCC("data.item[%d].floatCoe = %d\n", i, data.item[i].floatCoe);
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
    dar = data_to_buf_class13(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class13 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_cfg_8104(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        {
            uint32 i = 0;
            OOP_8104_CONFIG_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].defValue = %lld\n", i, data.item[i].defValue);
                PRTL_FMT_LOGCC("data.item[%d].startTime.year = %d\n", i, data.item[i].startTime.year);
                PRTL_FMT_LOGCC("data.item[%d].duration = %d\n", i, data.item[i].duration);
                PRTL_FMT_LOGCC("data.item[%d].powLimit = 0x%x\n", i, data.item[i].powLimit);
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
    dar = data_to_buf_class13(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class13 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_cfg_8105(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        {
            uint32 i = 0;
            OOP_8105_CONFIG_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].startTime.year = %d\n", i, data.item[i].startTime.year);
                PRTL_FMT_LOGCC("data.item[%d].endTime.year = %d\n", i, data.item[i].endTime.year);
                PRTL_FMT_LOGCC("data.item[%d].defValue = %lld\n", i, data.item[i].defValue);
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
    dar = data_to_buf_class13(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class13 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_cfg_8107_nor(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        {
            uint32 i = 0;
            OOP_8107_CONFIG_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].number = %d\n", i, data.item[i].number);
                PRTL_FMT_LOGCC("data.item[%d].flag = %d\n", i, data.item[i].flag);
                PRTL_FMT_LOGCC("data.item[%d].type = %d\n", i, data.item[i].type);
                PRTL_FMT_LOGCC("data.item[%d].purElectricity = %lld\n", i, data.item[i].purElectricity);
                PRTL_FMT_LOGCC("data.item[%d].alarmLimit = %lld\n", i, data.item[i].alarmLimit);
                PRTL_FMT_LOGCC("data.item[%d].tripLimit = %lld\n", i, data.item[i].tripLimit);
                PRTL_FMT_LOGCC("data.item[%d].mode = %d\n", i, data.item[i].mode);
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
    dar = data_to_buf_class13(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class13 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_cfg_8107_rec(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, 
    RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 i = 0;
    uint16 j = 0;
    OOP_DAR_E  nRet = DATA_OTHER_REASON;
    uint32 offset = 4;
    uint16 csdStart = 0, csdLen = 0;
    uint16 outOffset = 0;
    uint16 inDataLenTmp = InDataLen;

    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;
    uint16     headLen = 0;
    READ_NOR_BP_T inReadNor;
    RELY_INFO_T   relyInfo;
    RECORD_BUF_T  outRecordBuf;

    /* ��0���� */
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    MEMZERO(&inReadNor, sizeof(inReadNor));
    MEMZERO(&relyInfo, sizeof(relyInfo));
    MEMZERO(&outRecordBuf, sizeof(outRecordBuf));

    /* ���Ͳ����� */
    if (pOadInfo->pTab->oad.value != 0x81070200)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);

    /*����RSD*/
    nRet = apdu_rsd_fromfmt698(inData, inDataLenTmp, &offset, &oopRsd, sizeof(oopRsd));
    PRTL_FMT_DEBUG("ɸѡ����[%d]\n", oopRsd.choice);
    switch (oopRsd.choice)
    {
    case 1:
        {
            PRTL_FMT_DEBUG("����1 OAD = 0x%08x\n", oopRsd.sel1.oad.value);
            PRTL_FMT_DEBUG("����1 type = %d\n", oopRsd.sel1.value.type);
            PRTL_FMT_DEBUG("����1 value = 0x%04x\n", oopRsd.sel1.value.oi);
        }
        break;
    default:
        break;
    }

    if (nRet != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD��������\n");

        /*Ӧ�Ե��ԺЭ��һ���Բ���*/
        uint16 testOffset = 4;
        uint16 inCsdOff = 0, inCsdLen = 0;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RSD, TRUE);
        inCsdOff = testOffset;
        apdu_next_dpos_get(inData, &testOffset, InDataLen, DT_RCSD, TRUE);
        inCsdLen = testOffset - inCsdOff;
        memcpy(outData->pResultData, inData, 4);
        memcpy(outData->pResultData + 4, inData + inCsdOff, inCsdLen);
        outData->resultDataLen = 4 + inCsdLen;

        outData->pResultData[outData->resultDataLen++] = 1;                   //�����Ż��£�outData->resultDataLen������ͳһ
        if (apdu_rcsd_fromfmt698((uint8 *)inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
        {
            outData->pResultData[outData->resultDataLen++] = 0;

        }
        else
        {
            outData->pResultData[outData->resultDataLen++] = 1;
            for (i = 0; i < oopRcsd.nNum; i++)
            {
                outData->pResultData[outData->resultDataLen++] = NULL_DATA;
            }
        }

        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_SUCCESS;
    }
    else
    {
        inDataLenTmp -= offset;  //��ȥRSD��OAD��ĳ���
    }

    /*����RCSD*/
    csdStart = offset;
    if (apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd)) != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("CSD��������!\n");
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_TYPE_UNMATCHED;
    }

    /*��дOAD+RCSD+��¼���1 ������ƫ��headLen*/
    outData->resultDataLen = 0;
    csdLen = offset-csdStart;
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    if (csdLen == 1)
    {
        //���rcsdΪ�գ������OAD
        outData->pResultData[outOffset ++] = 0x01;
        outData->pResultData[outOffset ++] = 0x00;
        outData->pResultData[outOffset ++] = 0x81;
        outData->pResultData[outOffset ++] = 0x0C;
        outData->pResultData[outOffset ++] = 0x02;
        outData->pResultData[outOffset ++] = 0x00;
    }
    else
    {
        memcpy(&outData->pResultData[outOffset], &inData[csdStart], csdLen);
        outOffset += csdLen;
    }
    outData->pResultData[outOffset++] = 1;
    outData->resultDataLen += outOffset;
    headLen = outOffset;
    PRTL_BUF_LOGCC(&outData->pResultData[0], headLen, "��дOAD+RCSD+��¼���1 ������ƫ��headLen head:");

    /*��¼�͵����ӿ��ṩ�������*/
    if (read_nor_param_get(&oopRsd, &oopRcsd, pApduInfo, &inReadNor, &relyInfo) != ERR_OK)
    {
        free(outRecordBuf.recordBuf);
        outRecordBuf.recordBuf = NULL;
        return DATA_READ_WRITE_DENIED;
    }

    /* ������Ϣ�д���OAD�б��ָ�� */
    relyInfo.pOadInfo = pOadInfo;

    PRTL_FMT_DEBUG("******************ת���ڲ�˧ѡ������ʼ********************\n");
    PRTL_FMT_DEBUG("ɸѡ��ʼ[%d]\n", inReadNor.startInfoNum);
    PRTL_FMT_DEBUG("ɸѡ����[%d]\n", inReadNor.endInfoNum);
    PRTL_FMT_DEBUG("ɸѡ��������[%d]\n", relyInfo.oopRcsd.nNum);
    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        if (relyInfo.oopRcsd.cols[i].choice == 0)
        {
            PRTL_FMT_DEBUG("��OAD relyInfo.oopRcsd.cols[%d].oad.value = 0x%08x\n", i, relyInfo.oopRcsd.cols[i].oad.value);
        }
        else if(relyInfo.oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_DEBUG("����OAD relyInfo.oopRcsd.cols[%d].road.oadMain.value = 0x%08x\n", i, 
                relyInfo.oopRcsd.cols[i].road.oadMain.value);
            for (j = 0; j < relyInfo.oopRcsd.cols[i].road.oadCols.nNum; j++)
            {
                PRTL_FMT_DEBUG("��OAD relyInfo.oopRcsd.cols[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j, 
                    relyInfo.oopRcsd.cols[i].road.oadCols.oad[j].value);
            }
        }
    }
    PRTL_FMT_DEBUG("******************ת���ڲ�˧ѡ��������********************\n");

    /*����¼�ӿڣ����Զ���apdu�����򣬿ɷ��ضϵ���Ϣ*/
    if (record_cfg_8107_rec_read(pApduInfo->hUdp, inReadNor, relyInfo, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        nRet = DATA_READ_WRITE_DENIED;
    }
    else
    {
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;

        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            /*���¶ϵ���Ϣ*/
            PRTL_FMT_LOGCC("���¶ϵ���Ϣ\n");
            outRecordBuf.breakInfo.objPre = 0x02;   //�������ԣ���¼������
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*����ϵ���Ϣ*/
            PRTL_FMT_LOGCC("����ϵ���Ϣ\n");
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        nRet = DATA_SUCCESS;
    }
    
    free(outRecordBuf.recordBuf);
    outRecordBuf.recordBuf = NULL;
    return DATA_SUCCESS;
}

/**
*********************************************************************
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_cfg_8107(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    //�����Ӧ�÷�֡״̬����apdu[1]�޸ĳ���ʼ֡��ʽ
    if (pApduInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        pApduInfo->reqType = pApduInfo->bpBreakInfo->reqType;
    }

    switch (pApduInfo->reqType)
    {
    case 0x01:
    case 0x02:
        return class13_invoke_get_cfg_8107_nor(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x03:
    case 0x04:
        return class13_invoke_get_cfg_8107_rec(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    default:
        break;
    }

    return DATA_OBJECT_UNDEFINED;
}

/**
*********************************************************************
* @name��      class13_invoke_get
* @brief��     class13��GET����
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
OOP_DAR_E class13_invoke_get_cfg_8108(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 offset  = 0;
    uint32 len = 0;   

    MEMZERO(&convert, sizeof(convert)); 

    //��������
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        {
            uint32 i = 0;
            OOP_8108_CONFIG_T data = {0};
            normal.oad.value = pOadInfo->pTab->oad.value;
            ret = db_read_nomal(pApduInfo->hUdp, &normal, pOadInfo->pTab->dataLen, (uint8*)&data, &len);
            if ((ret != ERR_OK) || (len == 0))
            {
                PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, normal.logicId, normal.infoNum, normal.oad.value);
                memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
                offset += sizeof(OOP_OAD_U);
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x01;
                outData->pResultData[offset ++] = 0x00;
                outData->resultDataLen += offset;
                return DATA_SUCCESS;
            }

            for (i = 0; i < data.nNum; i++)
            {
                PRTL_FMT_LOGCC("data.item[%d].object = 0x%08x\n", i, data.item[i].object);
                PRTL_FMT_LOGCC("data.item[%d].defValue = %lld\n", i, data.item[i].defValue);
                PRTL_FMT_LOGCC("data.item[%d].limitCoe = %d\n", i, data.item[i].limitCoe);
                PRTL_FMT_LOGCC("data.item[%d].floatCoe = %d\n", i, data.item[i].floatCoe);
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
    dar = data_to_buf_class13(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class13 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
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
* @name��       class13_invoke_get
* @brief��      class13��GET����
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
OOP_DAR_E class13_invoke_get_8103(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return class13_invoke_get_cfg_8103(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x03:
        return class13_invoke_get_ctl_in_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_get_ctl_out_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x05:
        return class13_invoke_get_ctl_alarm_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_get
* @brief��      class13��GET����
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
OOP_DAR_E class13_invoke_get_8104(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return class13_invoke_get_cfg_8104(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x03:
        return class13_invoke_get_ctl_in_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_get_ctl_out_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x05:
        return class13_invoke_get_ctl_alarm_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}


/**
*********************************************************************
* @name��       class13_invoke_get
* @brief��      class13��GET����
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
OOP_DAR_E class13_invoke_get_8105(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return class13_invoke_get_cfg_8105(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x03:
        return class13_invoke_get_ctl_in_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_get_ctl_out_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x05:
        return class13_invoke_get_ctl_alarm_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_get
* @brief��      class13��GET����
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
OOP_DAR_E class13_invoke_get_8106(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    // case 0x02:
    //     return class13_invoke_get_cfg_8105(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
    //     break;
    case 0x03:
        return class13_invoke_get_ctl_in_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_get_ctl_out_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x05:
        return class13_invoke_get_ctl_alarm_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_get
* @brief��      class13��GET����
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
OOP_DAR_E class13_invoke_get_8107(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return class13_invoke_get_cfg_8107(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x03:
        return class13_invoke_get_ctl_in_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_get_ctl_out_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x05:
        return class13_invoke_get_ctl_alarm_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_get
* @brief��      class13��GET����
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
OOP_DAR_E class13_invoke_get_8108(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.attID)
    {
    case 0x02:
        return class13_invoke_get_cfg_8108(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x03:
        return class13_invoke_get_ctl_in_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x04:
        return class13_invoke_get_ctl_out_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x05:
        return class13_invoke_get_ctl_alarm_state(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

#if DESC("class13����ӿں���",1)

/**
*********************************************************************
* @name��       class13_invoke_set
* @brief��      class13��SET����
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
OOP_DAR_E class13_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = 0;

    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x8103:
        ret = class13_invoke_set_ctl_8103(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0x8104:
        ret = class13_invoke_set_ctl_8104(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;  
    case 0x8105:
        ret = class13_invoke_set_ctl_8105(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x8107:
        ret = class13_invoke_set_ctl_8107(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    case 0x8108:
        ret = class13_invoke_set_ctl_8108(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    /* ���óɹ���������ʾ */
    if(0 == ret)
    {
#if defined PRODUCT_ZCU_1 || defined PRODUCT_SEU
         appctrl_alarm_data(255, 0, 3, 1, NULL);
#endif
        appmain_buzzer_status(pApduInfo->hUdp, TRUE);
        usleep(100000);
        appmain_buzzer_status(pApduInfo->hUdp, FALSE);
    }
    return ret;
}

/**
*********************************************************************
* @name��       class13_invoke_get
* @brief��      class13��GET����
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
OOP_DAR_E class13_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x8103:
        return class13_invoke_get_8103(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x8104:
        return class13_invoke_get_8104(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x8105:
        return class13_invoke_get_8105(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x8106:
        return class13_invoke_get_8106(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x8107:
        return class13_invoke_get_8107(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    case 0x8108:
        return class13_invoke_get_8108(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        break;
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }
}

/**
*********************************************************************
* @name��       class13_invoke_act
* @brief��      class13��ACTION����
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
OOP_DAR_E class13_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = 0;

    switch (pOadInfo->pTab->oad.nObjID)
    {
    case 0x8103:
        ret = class13_invoke_act_8103(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break;  
    case 0x8104:
        ret = class13_invoke_act_8104(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break;  
    case 0x8105:
        ret = class13_invoke_act_8105(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
    case 0x8106:
        ret = class13_invoke_act_8106(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
    case 0x8107:
        ret = class13_invoke_act_8107(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
    case 0x8108:
        ret = class13_invoke_act_8108(pOadInfo, pApduInfo, inData, (uint16)InDataLen, outData, outDataMax);
        break; 
    default:
        return DATA_OBJECT_UNAVAILABLE;
        break;
    }

    /* ���óɹ���������ʾ */
    if(0 == ret)
    {
#if defined PRODUCT_ZCU_1 || defined PRODUCT_SEU
        if(0x3 == pOadInfo->pTab->oad.attID || 0x4 == pOadInfo->pTab->oad.attID || 0x5 == pOadInfo->pTab->oad.attID )
        {
            appctrl_alarm_data(255, 0, 3, 1, NULL);
        }
#endif
        appmain_buzzer_status(pApduInfo->hUdp, TRUE);
        usleep(100000);
        appmain_buzzer_status(pApduInfo->hUdp, FALSE);
    }
    return ret;
}


#endif
