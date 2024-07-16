/*
*********************************************************************
* @file    class9.c
* @brief�� class9����
* @author��
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"
#include "ac_sample.h"
#include "ac_main.h"
#include "oopStorage.h"

//
// class9
//

/**
*********************************************************************
* @brief��      ��ȡ��������м�¼��
* @param[in]�� pRSD          - ��ѡ��
               pCSD          - ��ѡ��
* @param[out]��pPlanTypeInfo - Ŀ������
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int32 frozen_all_rcsd_get(OAD_INFO_T *pOadInfo, DB_CLIENT fd, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax)
{
    int32 ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;
    uint32 i = 0;
    OOP_FROZENSETS_T data;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0; 
    OOP_RCSD_T  oopRcsd;   
    
    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&oopRcsd, sizeof(oopRcsd)); 

    //��������OAD����
    normal.logicId = 1;
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.oad.attID = 3;
    
    ret = db_read_nomal(fd, &normal, sizeof(OOP_FROZENSETS_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        ACUDP_FMT_DEBUG("frozen_all_rcsd_get failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
    }

    oopRcsd.nNum = 2 + data.nNum;
    oopRcsd.cols[0].oad.value = 0x20230200;
    oopRcsd.cols[1].oad.value = 0x20210200;

    for(i = 0; i< data.nNum; i++)
    {
        oopRcsd.cols[i+2].oad.value = data.objs[i].oad.value;
    }

    if(pOadInfo->noParse)
    {
        memcpy(&pOutData[offset], &oopRcsd, sizeof(oopRcsd));
        offset += sizeof(oopRcsd);
    }
    else
    {
        dar = apdu_rcsd_tofmt698(pOutData, outDataMax, &offset, &oopRcsd, sizeof(oopRcsd));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_DEBUG("apdu_rcsd_tofmt698 failed. oopRcsd.nNum(%d)\n", oopRcsd.nNum);
            return dar;        
        }
    }
    
    *pOffset = offset;
    
    return ret;
}

/**
*********************************************************************
* @name��       frozen_cfg_cmp
* @brief��      �Ƚ϶�������Ƿ���ͬ
* @param[in] ��pOdaInfo        OAD��Ϣ
               inData         ���뱨��
               inLen          ���뱨�ĳ���
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2019-12-6
*********************************************************************
*/
int frozen_cfg_cmp(void *vl, void *vr)
{
    OOP_FROZENOBJ_T *lvalue = (OOP_FROZENOBJ_T *)vl;
    OOP_FROZENOBJ_T *rvalue = (OOP_FROZENOBJ_T *)vr;

    if(lvalue->oad.value == rvalue->oad.value)
    {
        return 0;
    }

    return -1;
}


/**
*********************************************************************
* @name��       frozen_cfg_unit_del
* @brief��      ɾ��һ�������������
* @param[in] ��pOdaInfo        OAD��Ϣ
               inData         ���뱨��
               inLen          ���뱨�ĳ���
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_cfg_unit_del(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_FROZENSETS_T data;
    OOP_OAD_U delID;
    CLEAR_DATA_T clearData;
    uint32 sOffset = 4;
    uint32 len = 0;
    uint32 i = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&clearData, sizeof(clearData));     
    MEMZERO(&delID, sizeof(delID)); 

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    //�ӱ�������ȡ����
    dar = basic_buf_to_data(E_OAD, inData, InDataLen, &sOffset, &delID, sizeof(delID));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(inData, InDataLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    normal.oad.value = oad.value;
    normal.logicId = 1;//��������

    //��ȡԭ�����ñ�,��������Ϊ�Ѿ�ɾ��,ֱ�ӷ��سɹ�
    ret = db_read_nomal(gDbUdpClient, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SUCCESS;
    }

    //�ҵ���Ӧ�Ĺ�������,��λ�����ķ�·��ǰ�ݲ��ƶ�
    for(i = 0; i < data.nNum; i++)
    {
        if(data.objs[i].oad.value == delID.value)
        {
            memmove(&data.objs[i], &data.objs[i+1], (data.nNum-i-1)*sizeof(OOP_FROZENOBJ_T));
            data.nNum--;
            break;
        }
    }

    //�������Ϊ0����ɾ����OAD
    if(data.nNum == 0)
    {
        clearData.recordOAD.classtag = CLASS_DATA_INIT;
        clearData.recordOAD.road.oadCols.nNum = 1;
        clearData.recordOAD.road.oadCols.oad[0].value = oad.value;
        clearData.recordOAD.logicId = 1;//��������
        
        ret = db_clear_oad_data(gDbUdpClient, &clearData);
        if(ret != ERR_OK)
        {
            ACUDP_FMT_TRACE("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
            return DATA_OTHER_REASON;
        }
    }
    else
    {
        //д������
        normal.classtag = CLASS_DATA_INIT;
        normal.logicId = 1;//��������
        
        ret = db_write_nomal(gDbUdpClient, &normal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            ACUDP_FMT_TRACE("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }
    }
    
    return dar;
}

/**
*********************************************************************
* @name��       frozen_objattr_batch_add
* @brief��      ������Ӷ����������
* @param[in] ��fd           -�������ľ��
               frzOAD      -������OAD
               frzSets     -�������OAD
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2020-04-09
*********************************************************************
*/
int frozen_objattr_batch_add(DB_CLIENT fd, OOP_OAD_U frzOAD, OOP_FROZENSETS_T *frzSets)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    OOP_FROZENSETS_T data;
    OOP_FROZENSETS_T subData;
    uint32 len = 0;

    MEMZERO(&data, sizeof(data)); 
    memcpy_s(&subData, sizeof(OOP_FROZENSETS_T), frzSets, sizeof(OOP_FROZENSETS_T));

    normal.oad.value = frzOAD.value;
    normal.logicId = 1;

    //д��Ԫ���ݣ��ȶ�ȡȫ����Ϣ��ûд�Ͷ���ʧ�ܣ����Բ�������ֵ
    ret = db_read_nomal(fd, &normal, sizeof(data), (uint8*)&data, &len);
    ACUDP_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
        ret, normal.logicId, normal.infoNum, normal.oad.value);

    //ȥ��data��������subData���ظ�Ԫ��
    arrdsingle(&subData.objs, (int*)&subData.nNum, &data.objs, (int*)&data.nNum, sizeof(OOP_FROZENOBJ_T), frozen_cfg_cmp);

    //δ��������������ӣ�������������Խ��
    if((data.nNum + subData.nNum) <= OOP_MAX_FROZENOBJ)
    {
        memcpy(&data.objs[data.nNum], &subData.objs, sizeof(OOP_FROZENOBJ_T)*subData.nNum);
        data.nNum += subData.nNum;
    }
    else
    {
        ACUDP_FMT_TRACE("frozen_cfg_batch_add failed, member is full. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    //д������
    normal.classtag = CLASS_DATA_INIT;
    normal.logicId = 1;
    
    ret = db_write_nomal(fd, &normal, (uint8*)&data, sizeof(data));
    if(ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return ret;
}


/**
*********************************************************************
* @brief��      ��������������Ե�Ԫ
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
* @author    : ����
* @Date      ��2019-12-6                 
*********************************************************************
*/
OOP_DAR_E buf_to_data_frozen_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_FROZENOBJ_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    
    //�ṹ����
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //�ṹ��Ա��
    if(srcBuf[sOffset++] != 3)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //��������
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nPeriod, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //������������
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.oad, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //�洢���
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nDepth, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }        

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @brief��      ��������������Ա�
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcBuf       - Դ����
               srcLen       - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen       - Ŀ�����ݳ���
               sOffset      - ת����Դ��������ƫ��
               dOffset      - ת����Ŀ����������ƫ��
* @return��     =0           - �ɹ�
              !=0           - ������
* @author    : ����
* @Date      ��2019-12-6                 
*********************************************************************
*/
OOP_DAR_E buf_to_data_frozen_attr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_FROZENSETS_T data;
    uint8 offlen = 0;      //������������
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 unitDoffset = 0;
    DATA_CONVERT_T unitConvert;

    MEMZERO(&data, sizeof(data));
    memcpy(&unitConvert, convert, sizeof(DATA_CONVERT_T));
    
    //��������
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        ACUDP_BUF_TRACE(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //�����Ա����
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    *unitConvert.sOffset = sOffset;
    for(i = 0; i < data.nNum; i++)
    {
        unitDoffset = 0;
        unitConvert.dstBuf = &data.objs[i];
        unitConvert.dstLen = sizeof(OOP_FROZENOBJ_T);
        unitConvert.dOffset = &unitDoffset;

        dar = buf_to_data_frozen_unit(pOadInfo, &unitConvert);
        if(dar != DATA_SUCCESS)
        {
            ACUDP_BUF_TRACE(&srcBuf[sOffset], srcLen-sOffset, "buf_to_data_frozen_unit failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //����ÿ�����жϣ�������飬�����ǰ�����Ѿ�Խ��
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        ACUDP_FMT_TRACE("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}


/**
*********************************************************************
* @name��       frozen_cfg_batch_add
* @brief��      ������Ӷ����������
* @param[in] ��pOdaInfo        OAD��Ϣ
               inData         ���뱨��
               inLen          ���뱨�ĳ���
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_cfg_batch_add(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_FROZENSETS_T subData;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&subData, sizeof(subData)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &subData;
    convert.dstLen = sizeof(subData);
    convert.dOffset = &dOffset;  

    //�ӱ�������ȡ����
    dar = buf_to_data_frozen_attr(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(&inData[sOffset], InDataLen-sOffset, "buf_to_data_frozen_attr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    ret = frozen_objattr_batch_add(gDbUdpClient, oad, &subData);
    if(ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("frozen_cfg_batch_add failed. ret(%d)\n", ret);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name��       frozen_rectable_rec_get
* @brief��      ����¼��ʽ��ȡ�����¼��
* @param[in] ��pOdaInfo        OAD��Ϣ
               inData         ���뱨��
               inLen          ���뱨�ĳ���
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_rectable_rec_get(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    int nRet = ERR_OK;
    uint32 offset = 0;   //���뱨��buf��ƫ��
    uint32 csdStart = 0;
    RECORD_INFO_T recordInfo;
    BUFFER_INFO_T outRecord;
    OOP_OAD_U oopOad;
    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;   
    uint16 innerOffset = 0;
    OAD_INFO_T oadInfo;

    //������ӡ
    ACUDP_BUF_TRACE(inData, InDataLen, "frozen_rectable_rec_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

    //��ʼ��
    MEMZERO(&recordInfo, sizeof(recordInfo));
    MEMZERO(&outRecord, sizeof(outRecord));
    MEMZERO(&oopOad, sizeof(oopOad));
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    memcpy(&oadInfo, pOadInfo, sizeof(OAD_INFO_T));

    //��������ȡOAD
    memcpy_r(&oopOad, &inData[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //��������ȡRSD
    dar = apdu_rsd_fromfmt698(inData, InDataLen, &offset, &oopRsd, sizeof(oopRsd));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("RSD��������\n");
        return dar;
    }

    //��������ȡRCSD
    csdStart = offset;
    dar = apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("RCSD��������\n");
        return dar;
    }

    if(oopRcsd.nNum == 0)
    {
        //TODO,��������
        oadInfo.noParse = TRUE;
        all_rcsd_get(&oadInfo, gDbUdpClient, &innerOffset, (uint8*)&oopRcsd, sizeof(OOP_RCSD_T));
    }

    //�ȷ�װ��¼��Ӧͷ
    read_record_response(&oadInfo, gDbUdpClient, inData, InDataLen, offset, csdStart, NULL, outData, outDataMax);

    //�����ȡ��¼�Ŀռ�
    recordInfo.oopOad = oopOad;
    recordInfo.oopRsd = oopRsd;   
    recordInfo.oopRcsd = oopRcsd;
    recordInfo.append.pOadInfo = pOadInfo;
  
    outRecord.buffer = outData->pResultData;  
    outRecord.offset = &outData->resultDataLen;
    outRecord.bufLen = outDataMax;
    
    /*��ͳ�Ƽ�¼��ӿڣ����Զ���apdu�����򣬿ɷ��ضϵ���Ϣ*/
    nRet = record_frozen_read(gDbUdpClient, &recordInfo, &outRecord);
    if (nRet != 0)
    {
        ACUDP_FMT_DEBUG("record_tmn_frozen_read failed. nRet(%d)\n", nRet);    
        return nRet;
    }
    
    return dar;
}


/**
*********************************************************************
* @name��       class9_invoke_get
* @brief��      class9��GET����
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
OOP_DAR_E class9_invoke_get(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 2:   //�����¼��
        {
            dar = frozen_rectable_rec_get(pOadInfo, inData, InDataLen, outData, outDataMax);
        }break;   
                
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}


