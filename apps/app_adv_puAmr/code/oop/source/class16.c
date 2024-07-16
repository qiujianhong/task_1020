/*
*********************************************************************
* @file    class16.c
* @brief�� class16����
* @author��
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"

//
// class16
//

#if DESC("��ֵͳ�ƻ�����ԪD2B",1)

/**
*********************************************************************
* @brief��      �ۼ�ƽ��ͳ�ƽ����
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
OOP_DAR_E data_to_buf_extreme_result(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_EXTREMERES_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //�ṹ����
    dstBuf[dOffset++] = DT_STRUCTURE;

    //�ṹ��Ա��
    dstBuf[dOffset++] = 5;
            
    //������������
    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.obj, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.obj.value);
        return DATA_TYPE_UNMATCHED;
    }    

    //���ֵ
    dar = digit_to_fmt698(dstBuf, dstLen, &dOffset, &data.dbMaxValue, sizeof(OOP_DIGIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), choice(%d)\n", dar, data.dbMaxValue.choice);
        return DATA_TYPE_UNMATCHED;
    }

    //����ʱ��
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.tmMax, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.obj.value);
        return DATA_TYPE_UNMATCHED;
    } 
    
    //��Сֵ
    dar = digit_to_fmt698(dstBuf, dstLen, &dOffset, &data.dbMinValue, sizeof(OOP_DIGIT_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), choice(%d)\n", dar, data.dbMinValue.choice);
        return DATA_TYPE_UNMATCHED;
    }       

    //����ʱ��
    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.tmMin, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.obj.value);
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
* @brief��      �ۼ�ƽ��ͳ�ƽ����
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
OOP_DAR_E data_to_buf_extreme_results(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_EXTREMERESS_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
	uint16 i=0;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

	//�ṹ����
    dstBuf[dOffset++] = DT_ARRAY;
	//����
	dstBuf[dOffset++] = data.nNum;

	for(i=0;i<data.nNum;i++)
	{
	    //�ṹ����
	    dstBuf[dOffset++] = DT_STRUCTURE;

	    //�ṹ��Ա��
	    dstBuf[dOffset++] = 5;
	            
	    //������������
	    dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.extremvalue[i].obj, sizeof(OOP_OAD_U));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.extremvalue[i].obj.value);
	        return DATA_TYPE_UNMATCHED;
	    }    

	    //���ֵ
	    dar = digit_to_fmt698(dstBuf, dstLen, &dOffset, &data.extremvalue[i].dbMaxValue, sizeof(OOP_DIGIT_T));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), choice(%d)\n", dar, data.extremvalue[i].dbMaxValue.choice);
	        return DATA_TYPE_UNMATCHED;
	    }

	    //����ʱ��
	    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.extremvalue[i].tmMax, sizeof(OOP_DATETIME_S_T));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.extremvalue[i].obj.value);
	        return DATA_TYPE_UNMATCHED;
	    } 
	    
	    //��Сֵ
	    dar = digit_to_fmt698(dstBuf, dstLen, &dOffset, &data.extremvalue[i].dbMinValue, sizeof(OOP_DIGIT_T));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("digit_to_fmt698 failed. dar(%d), choice(%d)\n", dar, data.extremvalue[i].dbMinValue.choice);
	        return DATA_TYPE_UNMATCHED;
	    }       

	    //����ʱ��
	    dar = basic_data_to_buf(E_DATETIME_S, dstBuf, dstLen, &dOffset, &data.extremvalue[i].tmMin, sizeof(OOP_DATETIME_S_T));
	    if(dar != DATA_SUCCESS)
	    {
	        PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d),oad(0x%08x)\n", dar, data.extremvalue[i].obj.value);
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


#endif

#if DESC("��ֵͳ�ƻ�����ԪB2D",1)

#endif

#if DESC("����ת�������D2B&B2D",1)

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����
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
OOP_DAR_E data_to_buf_class16(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_EXTREMERES_T: //����ͳ�ƽ����
        {
            return data_to_buf_extreme_result(pOadInfo, convert);
        }break;
        
        case E_OOP_COGNATES_T: //�ۼ�ƽ��/��ֵͳ�ƹ����������Ա�
        {
            return data_to_buf_cognates_attr(pOadInfo, convert);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("data_to_buf_class16 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);            
        }break;        
    }
    
    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @brief��      ���������ĵ�����ת698��ʽ����
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
OOP_DAR_E buf_to_data_class16(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_COGNATES_T: //�ۼ�ƽ��/��ֵͳ�ƹ����������Ա�
        {
            return buf_to_data_cognates_attr(pOadInfo, convert);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("buf_to_data_class16 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);
        }break;        
    }
    
    return DATA_OBJECT_UNAVAILABLE;

}

#endif

#if DESC("SET�������ݽӿ�",1)

#endif

#if DESC("GET�������ݽӿ�",1)

#endif


#if DESC("class16����ӿں���",1)


const OOP_OAD_U g_tg2122_objs[]={//�ܼ����ռ�ֵ
    {0x23010300},

};

const OOP_OAD_U g_tg2123_objs[]={//�ܼ����¼�ֵ
    {0x23010300},
};

const OOP_OAD_U g_pulse2122_objs[]={//�����ռ�ֵ
    {0x24010500},
};

const OOP_OAD_U g_pulse2123_objs[]={//�����¼�ֵ
    {0x24010500},
};

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
int extreme_data_clear(DB_CLIENT fd, OOP_OAD_U extremeOAD)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData));
    
    //��������¼
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = extremeOAD.value;
    clearData.recordOAD.road.oadCols.oad[0].attID = 2;
    
    ret = db_clear_oad_data(fd, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return ret;
    }

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
int extreme_cfg_cmp(void *vl, void *vr)
{
    OOP_COGNATEOBJ_T *lvalue = (OOP_COGNATEOBJ_T *)vl;
    OOP_COGNATEOBJ_T *rvalue = (OOP_COGNATEOBJ_T *)vr;

    if(lvalue->obj.value == rvalue->obj.value)
    {
        return 0;
    }

    return -1;
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
int extreme_objattr_batch_add(DB_CLIENT fd, OOP_OAD_U extremeOAD, OOP_COGNATES_T *extremeSets)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    OOP_COGNATES_T data;
    OOP_COGNATES_T subData;
    uint32 len = 0;

    MEMZERO(&data, sizeof(data)); 
    memcpy_s(&subData, sizeof(OOP_COGNATES_T), extremeSets, sizeof(OOP_COGNATES_T));

    normal.oad.value = extremeOAD.value;

    //д��Ԫ���ݣ��ȶ�ȡȫ����Ϣ��ûд�Ͷ���ʧ�ܣ����Բ�������ֵ
    ret = db_read_nomal(fd, &normal, sizeof(data), (uint8*)&data, &len);
    PRTL_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
        ret, normal.logicId, normal.infoNum, normal.oad.value);

    //ȥ��data��������subData���ظ�Ԫ��
    arrdsingle(&subData.objs, (int*)&subData.nNum, &data.objs, (int*)&data.nNum, sizeof(OOP_COGNATEOBJ_T), extreme_cfg_cmp);

    //δ��������������ӣ�������������Խ��
    if((data.nNum + subData.nNum) <= OOP_COGNATE_OBJS)
    {
        memcpy(&data.objs[data.nNum], &subData.objs, sizeof(OOP_COGNATEOBJ_T)*subData.nNum);
        data.nNum += subData.nNum;
    }
    else
    {
        PRTL_FMT_DEBUG("extreme_objattr_batch_add failed, member is full. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    //д������
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(fd, &normal, (uint8*)&data, sizeof(data));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return ret;
}

/**
*********************************************************************
* @name��       frozen_tgday_objattr_add
* @brief��      �ܼ��������߶�������
* @param[in] ��fd           -�������ľ��
               tgOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2020-04-09
*********************************************************************
*/
int extreme_tgday_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_tg2122_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_tg2122_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = tgOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }

    //�����ֵ��¼
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_add(fd, extremeOAD, &extremeSets);
}
/**
*********************************************************************
* @name��       frozen_tgday_objattr_add
* @brief��      �ܼ��������߶�������
* @param[in] ��fd           -�������ľ��
               tgOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2020-04-09
*********************************************************************
*/
int extreme_tgmon_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_tg2123_objs;
    OOP_OAD_U extremeOAD = {0x21230300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_tg2123_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = tgOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }

    //�����ֵ��¼
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_add(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name��       extreme_objattr_add
* @brief��      �����ܼ���ʱĬ����ӵļ�ֵ����
* @param[in] ��fd           -�������ľ��
               tgOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��
*********************************************************************
*/
int extreme_tg_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    
    ret |= extreme_tgday_objattr_add(fd, tgOAD);
    ret |= extreme_tgmon_objattr_add(fd, tgOAD);

    return ret;
}

/**
*********************************************************************
* @name��       extreme_objattr_clear
* @brief��      
* @param[in] ��fd           -�������ľ��
               frzOAD      -������OAD
               frzSets     -�������OAD
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2020-04-09
*********************************************************************
*/
int extreme_objattr_clear(DB_CLIENT fd, OOP_OAD_U extremeOAD)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData));
    
    //��������¼
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = extremeOAD.value;
    
    ret = db_clear_oad_data(fd, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return ret;
    }

    return ret;
}


/**
*********************************************************************
* @name��       extreme_objattr_batch_del
* @brief��      
* @param[in] ��fd           -�������ľ��
               frzOAD      -������OAD
               frzSets     -�������OAD
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��
*********************************************************************
*/
int extreme_objattr_batch_del(DB_CLIENT fd, OOP_OAD_U extremeOAD, OOP_COGNATES_T *extremeSets)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    OOP_COGNATES_T data;
    uint32 len = 0;
    CLEAR_DATA_T clearData;
    uint8 i = 0, j = 0;

    MEMZERO(&clearData, sizeof(clearData));
    MEMZERO(&data, sizeof(data)); 

    normal.oad.value = extremeOAD.value;

    //��ȡԭ�����ñ�
    ret = db_read_nomal(fd, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    //�ҵ���Ӧ�Ĺ�������,��λ�����ķ�·��ǰ�ݲ��ƶ�
    for(i = 0; i < extremeSets->nNum; i++)
    {
        for(j = 0; j < data.nNum; j++)
        {
            if(data.objs[j].obj.value == extremeSets->objs[i].obj.value)
            {
                memmove(&data.objs[j], &data.objs[j+1], (data.nNum-j-1)*sizeof(OOP_COGNATEOBJ_T));
                data.nNum--;
                break;
            }
        }
    }
    
    //�������Ϊ0����ɾ����OAD
    if(data.nNum == 0)
    {
        clearData.recordOAD.classtag = CLASS_DATA_INIT;
        clearData.recordOAD.road.oadCols.nNum = 1;
        clearData.recordOAD.road.oadCols.oad[0].value = extremeOAD.value;
        
        ret = extreme_objattr_clear(fd, extremeOAD);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("extreme_objattr_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
            return ret;
        }
    }
    else
    {
        //д������
        normal.classtag = CLASS_DATA_INIT;
        
        ret = db_write_nomal(fd, &normal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return ret;
        }
    }

    return ret;
}

/**
*********************************************************************
* @name��       extreme_tgday_objattr_del
* @brief��      
* @param[in] ��fd           -�������ľ��
               tgOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    :
* @Date      
*********************************************************************
*/
int extreme_tgday_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_tg2122_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_tg2122_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = tgOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }


    //��������¼
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_del(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name��       extreme_tgmon_objattr_del
* @brief��      
* @param[in] ��fd           -�������ľ��
               tgOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    :
* @Date      
*********************************************************************
*/
int extreme_tgmon_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_tg2123_objs;
    OOP_OAD_U extremeOAD = {0x21230300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_tg2123_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = tgOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }


    //��������¼
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_del(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name��       extreme_tg_objattr_del
* @brief��     
* @param[in] ��fd           -�������ľ��
               tgOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    : 
* @Date      ��
*********************************************************************
*/
int extreme_tg_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    
    ret |= extreme_tgday_objattr_del(fd, tgOAD);
    ret |= extreme_tgmon_objattr_del(fd, tgOAD);

    return ret;
}

/**
*********************************************************************
* @name��       extreme_pulseday_objattr_add
* @brief��      ���������߶�������
* @param[in] ��fd           -�������ľ��
               pulseOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2020-04-09
*********************************************************************
*/
int extreme_pulseday_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_pulse2122_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_pulse2122_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = pulseOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }

    //�����ֵ��¼
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }


    return extreme_objattr_batch_add(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name��       extreme_pulseday_objattr_add
* @brief��      ���������߶�������
* @param[in] ��fd           -�������ľ��
               pulseOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2020-04-09
*********************************************************************
*/
int extreme_pulsemon_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_pulse2123_objs;
    OOP_OAD_U extremeOAD = {0x21230300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_pulse2123_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = pulseOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }

    //�����ֵ��¼
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }


    return extreme_objattr_batch_add(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name��       frozen_pulse_objattr_add
* @brief��      �ܼ��������߶�������
* @param[in] ��fd           -�������ľ��
               pulseOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2020-04-09
*********************************************************************
*/
int extreme_pulse_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;

    ret |= extreme_pulseday_objattr_add(fd, pulseOAD);
    ret |= extreme_pulsemon_objattr_add(fd, pulseOAD);

    return ret;
}

/**
*********************************************************************
* @name��       extreme_tgday_objattr_del
* @brief��      
* @param[in] ��fd           -�������ľ��
               tgOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    :
* @Date      
*********************************************************************
*/
int extreme_pulseday_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_pulse2122_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_pulse2122_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = pulseOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }


    //��������¼
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_del(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name��       extreme_tgday_objattr_del
* @brief��      
* @param[in] ��fd           -�������ľ��
               tgOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    :
* @Date      
*********************************************************************
*/
int extreme_pulsemon_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_COGNATES_T extremeSets;
    const OOP_OAD_U *pObjs = g_pulse2123_objs;
    OOP_OAD_U extremeOAD = {0x21220300};

    MEMZERO(&extremeSets, sizeof(extremeSets)); 

    extremeSets.nNum = sizeof(g_pulse2123_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < extremeSets.nNum; i++)
    {
        extremeSets.objs[i].obj.value =  pObjs[i].value; 
        extremeSets.objs[i].obj.nObjID = pulseOAD.nObjID;
        extremeSets.objs[i].period = 1; 
        extremeSets.objs[i].freq.unit = 1;
        extremeSets.objs[i].freq.value = 1;
    }


    //��������¼
    ret = extreme_data_clear(fd, extremeOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("extreme_data_clear failed. ret(%d), extremeOAD(0x%08x)\n", ret, extremeOAD.value);
    }

    return extreme_objattr_batch_del(fd, extremeOAD, &extremeSets);
}

/**
*********************************************************************
* @name��       frozen_pulse_objattr_add
* @brief��      �ܼ��������߶�������
* @param[in] ��fd           -�������ľ��
               pulseOAD       -�ܼ���OAD
* @return    ���������ʵ�ʳ���
* @author    : ����
* @Date      ��2020-04-09
*********************************************************************
*/
int extreme_pulse_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    ret |= extreme_pulseday_objattr_del(fd, pulseOAD);
    ret |= extreme_pulsemon_objattr_del(fd, pulseOAD);

    return ret;
}


/**
*********************************************************************
* @name��       class16_invoke_set
* @brief��      class16��SET����
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
OOP_DAR_E class16_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 3:   //�����������Ա�
        {
            dar = cognates_objattr_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}

/**
*********************************************************************
* @name��       class16_invoke_get
* @brief��      class16��GET����
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
OOP_DAR_E class16_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 2:   //��¼��
        {
            dar = stat_rectable_get(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        case 3:   //�����������Ա�
        {
            dar = cognates_objattr_get(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}

/**
*********************************************************************
* @name��       class16_invoke_act
* @brief��      class16��ACTION����
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
OOP_DAR_E class16_invoke_act(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint16 outLen = 0;

    return outLen;
}


#endif
