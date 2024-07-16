/*
*********************************************************************
* @file    class26.c
* @brief�� class26����
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
// class26
//
/**
*********************************************************************
* @brief��      ��ȡ��̫��ͨ�ŵ�ͨ������
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int32 data_to_buf_eth_config(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    uint32     i=0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_ETHCONFIG_T ETHCONFIG_T;
    
    memcpy((uint8*)&ETHCONFIG_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 8;
    //����ģʽ
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &ETHCONFIG_T.workmode, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  
    //���ӷ�ʽ
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &ETHCONFIG_T.contype, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    //����Ӧ�÷�ʽ
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &ETHCONFIG_T.conmode, 1) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    } 
    //�����˿��б�
    if (ETHCONFIG_T.listenNum > 4)
    {
        ETHCONFIG_T.listenNum = 0;
    }
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = ETHCONFIG_T.listenNum;
    for(i=0;i<ETHCONFIG_T.listenNum;i++)
    {
        if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &ETHCONFIG_T.listen[i], 2) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        } 
    }
    //�����������ַ
    dstBufTmp[offset++] = DT_OCTET_STRING;
    dstBufTmp[offset++] = 0x04;
    memcpy(dstBufTmp+offset,ETHCONFIG_T.proxyip,4);
    offset=offset+4;
    //����˿�
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &ETHCONFIG_T.proxyport, 2) < 0)
    {
            nRet = ERR_NORMAL;
            return nRet;
    }
    //��ʱʱ�估�ط�����
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &ETHCONFIG_T.dail.body, 1) < 0)
    {
            nRet = ERR_NORMAL;
            return nRet;
    }
    //��������
    if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &ETHCONFIG_T.heart, 2) < 0)
    {
            nRet = ERR_NORMAL;
            return nRet;
    }
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief��      ��ȡ��̫����������
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int32 data_to_buf_eth_net_config(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_ETHIP_T ETHIP_T;
    
    memcpy((uint8*)&ETHIP_T, srcData, srcLen);
    dstBufTmp[offset++]=DT_STRUCTURE;
    dstBufTmp[offset++]=0x06;
    //IP���÷�ʽ
    if(basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &ETHIP_T.ipConfig, 1) < 0)
    {
            nRet = ERR_NORMAL;
            return nRet;
    }
    //IP��ַ
    dstBufTmp[offset++] = DT_OCTET_STRING;
    dstBufTmp[offset++] = 0x04;
    memcpy(dstBufTmp+offset,ETHIP_T.ip,4);
    offset=offset+4;
    //��������
    dstBufTmp[offset++] = DT_OCTET_STRING;
    dstBufTmp[offset++] = 0x04;
    memcpy(dstBufTmp+offset,ETHIP_T.mask,4);
    offset=offset+4;
    //���ص�ַ
    dstBufTmp[offset++] = DT_OCTET_STRING;
    dstBufTmp[offset++] = 0x04;
    memcpy(dstBufTmp+offset,ETHIP_T.ipGateway,4);
    offset=offset+4;
    //PPPoE�û���
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = ETHIP_T.pppoeUser.nNum;
    if(ETHIP_T.pppoeUser.nNum > 0)
    {
        memcpy(dstBufTmp+offset,ETHIP_T.pppoeUser.value,ETHIP_T.pppoeUser.nNum);
        offset=offset+ETHIP_T.pppoeUser.nNum;
    }
    //PPPoE����
    dstBufTmp[offset++] = DT_VISIBLE_STRING;
    dstBufTmp[offset++] = ETHIP_T.pppoePwd.nNum;
    if(ETHIP_T.pppoePwd.nNum > 0)
    {
        memcpy(dstBufTmp+offset,ETHIP_T.pppoePwd.value,ETHIP_T.pppoePwd.nNum);
        offset=offset+ETHIP_T.pppoePwd.nNum;
    }
    *pOffset=offset;
    printf("ETHIP_T.pppoePwd.nNum offset is %d\n",offset);
    return ERR_OK;
}


/**
*********************************************************************
* @brief��      ��ȡ��̫��MAC��ַ
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int32 data_to_buf_eth_mac_addr(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
//    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_ETHMAC_T ETHMAC_T;
    
    memcpy((uint8*)&ETHMAC_T, srcData, srcLen);
    //MACD��ַ
    dstBufTmp[offset++] = DT_OCTET_STRING;
    dstBufTmp[offset++] = 0x06;
    memcpy(dstBufTmp+offset,ETHMAC_T.data,6);
    offset=offset+6;
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief��     ������̫��ͨ�ŵ�ͨ������
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int32 buf_to_data_eth_config(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo,void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_ETHCONFIG_T ETHCONFIG_T;
    uint32 i=0;
    uint32 offset=*pOffset;
    uint8 *srcDataTmp = (uint8 *)srcData;
    uint8 num;
    int result;
    
    if(srcDataTmp[offset++]!=DT_STRUCTURE)
    {
        return ERR_OPTION;
    }
    offset++;
    //����ģʽ
    result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&ETHCONFIG_T.workmode,1);
    //���ӷ�ʽ
    result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&ETHCONFIG_T.contype,1);
    //����Ӧ�÷�ʽ
    result=basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&ETHCONFIG_T.conmode,1);
    //�����˿��б�
    if(DT_NULL == srcDataTmp[offset])
    {
        offset++;
        ETHCONFIG_T.listenNum = 1;
    }
    else
    {
        if(srcDataTmp[offset++]!=DT_ARRAY)
        {
            return ERR_OPTION;
        }
        ETHCONFIG_T.listenNum=srcDataTmp[offset++];
        for(i=0;i<ETHCONFIG_T.listenNum;i++)
        {
            result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&ETHCONFIG_T.listen[i],2);
        }
    }
    
    //�����������ַ
    if(DT_NULL == srcDataTmp[offset])
    {
        offset++;
        ETHCONFIG_T.proxyip[0] = ETHCONFIG_T.proxyip[1] = ETHCONFIG_T.proxyip[2] = ETHCONFIG_T.proxyip[3] = 0;;
    }
    else
    {
        if(srcDataTmp[offset++]!=DT_OCTET_STRING)
        {
            return ERR_OPTION;
        }
        num=srcDataTmp[offset++];

        if(num>4)
        {
            return ERR_OPTION;
        }
        memcpy(ETHCONFIG_T.proxyip,srcDataTmp+offset,num);
        offset=offset+num;
    }

    //����˿�
    if(DT_NULL == srcDataTmp[offset])
    {
        offset++;
        ETHCONFIG_T.proxyport = 0;
    }
    else
    {
        result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&ETHCONFIG_T.proxyport,2);
    }
    //��ʱʱ�估�ط�����
    result=basic_buf_to_data(E_UNSIGNED,srcDataTmp,srcLen,&offset,&ETHCONFIG_T.dail.body,1);
    //�������ڣ��룩
    result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&ETHCONFIG_T.heart,2);

    memcpy((uint8*)dstBuf,(uint8*)&ETHCONFIG_T,sizeof(OOP_ETHCONFIG_T));
    *dstLen=sizeof(OOP_ETHCONFIG_T);
    return result;
}

/**
*********************************************************************
* @brief��     ������̫��ͨ�ŵ�ͨ������
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int32 buf_to_data_eth_heart_config(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo,void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_ETHCONFIG_T ETHCONFIG_T;
    uint32 offset=*pOffset;
    uint8 *srcDataTmp = (uint8 *)srcData;
    int result;
    int ret;
    uint32 len=0;
    
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = 0;
    NormalOad.oad.value = pOadInfo->pTab->oad.value&0xFFFFFF00;

    memset((uint8*)&ETHCONFIG_T,0x00,sizeof(OOP_ETHCONFIG_T));
    ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, sizeof(OOP_ETHCONFIG_T), (uint8*)&ETHCONFIG_T, &len);
    if((ret !=0)||(len !=sizeof(OOP_ETHCONFIG_T)))
    {
        
    }
    //�������ڣ��룩
    result=basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&ETHCONFIG_T.heart,2);

    memcpy((uint8*)dstBuf,(uint8*)&ETHCONFIG_T,sizeof(OOP_ETHCONFIG_T));
    *dstLen=sizeof(OOP_ETHCONFIG_T);
    return result;
}


int32 buf_to_data_eth_master_commu_param(void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_MASTERPARAMS_T MASTERPARAMS_T;
    uint32 i=0;
    uint32 offset=*pOffset;
    uint8 *srcDataTmp = (uint8 *)srcData;
    uint16 dataLen;
    if(srcDataTmp[offset++] !=DT_ARRAY)
    {
        return ERR_OPTION;
    }
    MASTERPARAMS_T.nNum=srcDataTmp[offset++];
    printf("MASTERPARAMS_T.nNum is %d\n",MASTERPARAMS_T.nNum);
    for(i=0;i<MASTERPARAMS_T.nNum;i++)
    {
        if(srcDataTmp[offset++] !=DT_STRUCTURE)
        {
            return ERR_OPTION;
        }
        offset++;
        if(srcDataTmp[offset++] !=DT_OCTET_STRING)
        {
            return ERR_OPTION;
        }
        dataLen = srcDataTmp[offset++];
        printf("dataLen is %d\n",dataLen);
        if(dataLen ==0x81)
        {
            dataLen = srcDataTmp[offset++];
        }
        else if(dataLen == 0x82)
        {
            dataLen = srcDataTmp[offset++];
            dataLen <<= 8;
            dataLen += srcDataTmp[offset++];
        }
        else if(dataLen>0x82||dataLen==0x80)
        {
            return ERR_OPTION;
        }
        //ip
        memcpy(MASTERPARAMS_T.master[i].ip,srcDataTmp+offset,dataLen);
        //port
        offset=offset+dataLen;
        basic_buf_to_data(E_LONG_UNSIGNED,srcDataTmp,srcLen,&offset,&MASTERPARAMS_T.master[i].port,2);    
    }
    memcpy((uint8*)dstBuf,(uint8*)&MASTERPARAMS_T,sizeof(OOP_MASTERPARAMS_T));
    *dstLen=sizeof(OOP_MASTERPARAMS_T);
    *pOffset=offset;
    return ERR_OK;


}

/**
*********************************************************************
* @brief��     ������̫��ͨ�ŵ���������
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int32 buf_to_data_eth_net_config(void *srcData, uint32 srcLen, uint32 *pOffset, void *dstBuf, uint32 *dstLen)
{
    OOP_ETHIP_T ETHIP_T;
    
    uint32 offset=*pOffset;
    uint8 *srcDataTmp = (uint8 *)srcData;
    uint16 dataLen=0;

    memset((uint8*)&ETHIP_T,0x00,sizeof(OOP_ETHIP_T));
    if(srcDataTmp[offset++]!=DT_STRUCTURE)
    {
        return DATA_TYPE_UNMATCHED;
    }
    offset++;
    //IP���÷�ʽ
    basic_buf_to_data(E_ENUM,srcDataTmp,srcLen,&offset,&ETHIP_T.ipConfig,1);
    //ip ��ַ
    if(srcDataTmp[offset++] !=DT_OCTET_STRING)
    {
        return DATA_TYPE_UNMATCHED;
    }
    dataLen = srcDataTmp[offset++];
    if(dataLen ==0x81)
    {
        dataLen = srcDataTmp[offset++];
    }
    else if(dataLen == 0x82)
    {
        dataLen = srcDataTmp[offset++];
        dataLen <<= 8;
        dataLen += srcDataTmp[offset++];
    }
    else if(dataLen>0x82||dataLen==0x80)
    {
        return DATA_TYPE_UNMATCHED;
    }
    memcpy(ETHIP_T.ip,srcDataTmp+offset,dataLen);
    offset=offset+dataLen;
    //��������
    if(srcDataTmp[offset++] !=DT_OCTET_STRING)
    {
        return DATA_TYPE_UNMATCHED;
    }
    dataLen = srcDataTmp[offset++];
    if(dataLen ==0x81)
    {
        dataLen = srcDataTmp[offset++];
    }
    else if(dataLen == 0x82)
    {
        dataLen = srcDataTmp[offset++];
        dataLen <<= 8;
        dataLen += srcDataTmp[offset++];
    }
    else if(dataLen>0x82||dataLen==0x80)
    {
        return DATA_TYPE_UNMATCHED;
    }
    memcpy(ETHIP_T.mask,srcDataTmp+offset,dataLen);
    offset=offset+dataLen;
    //���ص�ַ
    if(srcDataTmp[offset++] !=DT_OCTET_STRING)
    {
        return DATA_TYPE_UNMATCHED;
    }
    dataLen = srcDataTmp[offset++];
    printf("dataLen is %d,offset is %d\n",dataLen,offset);
    if(dataLen ==0x81)
    {
        dataLen = srcDataTmp[offset++];
    }
    else if(dataLen == 0x82)
    {
        dataLen = srcDataTmp[offset++];
        dataLen <<= 8;
        dataLen += srcDataTmp[offset++];
    }
    else if(dataLen>0x82||dataLen==0x80)
    {
        return DATA_TYPE_UNMATCHED;
    }
    memcpy(ETHIP_T.ipGateway,srcDataTmp+offset,dataLen);
    offset=offset+dataLen;
    //PPoE�û���
    if(srcDataTmp[offset]==DT_NULL)
    {
        offset++;
        ETHIP_T.pppoeUser.nNum= 0x00;
    }
    else if(srcDataTmp[offset] ==DT_VISIBLE_STRING)
    {
        offset++;
        dataLen = srcDataTmp[offset++];
        if(dataLen ==0x81)
        {
            dataLen = srcDataTmp[offset++];
        }
        else if(dataLen == 0x82)
        {
            dataLen = srcDataTmp[offset++];
            dataLen <<= 8;
            dataLen += srcDataTmp[offset++];
        }
        else if(dataLen>0x82||dataLen==0x80)
        {
            return DATA_TYPE_UNMATCHED;
        }
        ETHIP_T.pppoeUser.nNum=dataLen;
        printf("ETHIP_T.pppoeUser.nNum is %d\n",ETHIP_T.pppoeUser.nNum);
        memcpy(ETHIP_T.pppoeUser.value,srcDataTmp+offset,dataLen);
        offset=offset+dataLen;
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }
    
    //PPoE����
    if(srcDataTmp[offset]==DT_NULL)
    {
        offset++;
        ETHIP_T.pppoePwd.nNum= 0x00;
    }
    else if(srcDataTmp[offset] ==DT_VISIBLE_STRING)
    {
        offset++;
        dataLen = srcDataTmp[offset++];
        if(dataLen ==0x81)
        {
            dataLen = srcDataTmp[offset++];
        }
        else if(dataLen == 0x82)
        {
            dataLen = srcDataTmp[offset++];
            dataLen <<= 8;
            dataLen += srcDataTmp[offset++];
        }
        else if(dataLen>0x82||dataLen==0x80)
        {
            return DATA_TYPE_UNMATCHED;
        }
        printf("ETHIP_T.pppoePwd.nNum is %d\n",ETHIP_T.pppoePwd.nNum);
        ETHIP_T.pppoePwd.nNum=dataLen;
        memcpy(ETHIP_T.pppoePwd.value,srcDataTmp+offset,dataLen);
        offset=offset+dataLen;
    }
    else
    {
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset=offset;
    
    memcpy((uint8*)dstBuf,(uint8*)&ETHIP_T,sizeof(OOP_ETHIP_T));
    *dstLen=sizeof(OOP_ETHIP_T);
//    frame_printf(dstBuf, *dstLen);
    return ERR_OK;
}

/**
*********************************************************************
* @brief��      ��ȡͨ�ŵ���վͨ�Ų�����
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
int32 data_to_buf_eth_master_commu_param(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    int32 nRet = ERR_NORMAL;
    uint32     offset = *pOffset;
    uint32     i=0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    OOP_MASTERPARAMS_T MASTERPARAMS_T;
    
    memcpy((uint8*)&MASTERPARAMS_T, srcData, srcLen);
    dstBufTmp[offset++] = DT_ARRAY;
    dstBufTmp[offset++] = MASTERPARAMS_T.nNum;
    printf("get MASTERPARAMS_T.nNum is %d\n",MASTERPARAMS_T.nNum);
    for(i=0;i<MASTERPARAMS_T.nNum;i++)
    {
        dstBufTmp[offset++] = DT_STRUCTURE;
        dstBufTmp[offset++] = 2;
        //IP��ַ
        dstBufTmp[offset++] = DT_OCTET_STRING;
        dstBufTmp[offset++] = 0x04;
        memcpy(dstBufTmp+offset,MASTERPARAMS_T.master[i].ip,4);
        offset=offset+4;
        //�˿�
        if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &MASTERPARAMS_T.master[i].port, 2) < 0)
        {
            nRet = ERR_NORMAL;
            return nRet;
        }
    }
    *pOffset=offset;
    return ERR_OK;
}

/**
*********************************************************************
* @brief��     ���������ĵ�����ת698��ʽ����(class26)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E data_to_buf_class26(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}

/**
*********************************************************************
* @brief��      ���������ĵ�����ת698��ʽ����(class24)
* @param[in]�� ppOadInfo     - ���ݱ�ָ��
               srcData       - Դ����
               srcLen        - Դ���ݳ���
* @param[out]��dstBuf        - Ŀ������
               dstLen        - Ŀ�����ݳ���
* @return��    =0            - �ɹ�
               <0            - ������
*********************************************************************
*/
OOP_DAR_E buf_to_data_class26(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    return 0;
}


#if DESC("class26����ӿں���",1)

/**
*********************************************************************
* @name��       class26_invoke_set
* @brief��      class26��SET����
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
OOP_DAR_E class26_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret;
    uint32 len;
    uint32 offset=4;
    uint32 srcLen;
    int result;
    OOP_OAD_U oad;
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = 0;
    NormalOad.oad.value = pOadInfo->pTab->oad.value;
    oad.value=pOadInfo->pTab->oad.value;
    
    switch(oad.attID)
    {
        case 0x02:
        { 
            srcLen = sizeof(OOP_ETHCONFIG_T);
            uint8 buffer[srcLen];
            
            if (pOadInfo->nIndex == 0x00)
            {         
                result = buf_to_data_eth_config(pOadInfo,pApduInfo,(uint8 *)inData, inLen, &offset, buffer, &len);    
                if(result != ERR_OK)
                {
                    PRTL_FMT_DEBUG("class26_invoke_set buf_to_data_eth_config err[%d], oad[0x%08x]\n", 
                        result, pOadInfo->pTab->oad.value);
                    return result;
                }
            }
            else if (pOadInfo->nIndex == 0x08)
            {
                result = buf_to_data_eth_heart_config(pOadInfo, pApduInfo,(uint8 *)inData, inLen, &offset, buffer, &len);    
                if (result != ERR_OK)
                {
                    PRTL_FMT_DEBUG("class26_invoke_set buf_to_data_eth_heart_config err[%d], oad[0x%08x]\n", 
                        result, pOadInfo->pTab->oad.value);
                    return result;
                }
            }
            NormalOad.classtag = CLASS_DATA_UNINIT;
            ret = db_write_nomal(pApduInfo->hUdp, &NormalOad, buffer, len);
            if (ret != 0)
            {
                PRTL_FMT_DEBUG("class26_invoke_set db_write_nomal err[%d], oad[0x%08x]\n", 
                    ret, pOadInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }
        }
        break;
        case 0x03:
        { 

            srcLen = sizeof(OOP_MASTERPARAMS_T);
            uint8 buffer[srcLen];

            memset(buffer, 0x00, srcLen);
            result = buf_to_data_eth_master_commu_param((uint8 *)inData, inLen, &offset, buffer, &len);
            if (result != ERR_OK)
            {
                PRTL_FMT_DEBUG("class26_invoke_set buf_to_data_eth_master_commu_param err[%d], oad[0x%08x]\n", 
                    result, pOadInfo->pTab->oad.value);
                return result;
            }

            NormalOad.classtag = CLASS_DATA_UNINIT;
            ret = db_write_nomal(pApduInfo->hUdp, &NormalOad,buffer,len);
            if (ret < 0)
            {
                PRTL_FMT_DEBUG("class26_invoke_set db_write_nomal err[%d], oad[0x%08x]\n", 
                    ret, pOadInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }
            return DATA_SUCCESS;    
        }
        break;
        case 0x04:
        { 
            uint8 buffer[sizeof(OOP_ETHIP_T)];
            result = buf_to_data_eth_net_config((uint8 *)inData, inLen, &offset, buffer, &len);
            if (result != ERR_OK)
            {
                PRTL_FMT_DEBUG("class26_invoke_set buf_to_data_eth_net_config err[%d], oad[0x%08x]\n", 
                    result, pOadInfo->pTab->oad.value);
                return result;
            }

            NormalOad.classtag = CLASS_DATA_UNINIT;
            ret = db_write_nomal(pApduInfo->hUdp, &NormalOad, buffer, len);
            if (ret !=0)
            {
                PRTL_FMT_DEBUG("class26_invoke_set db_write_nomal err[%d], oad[0x%08x]\n", 
                    ret, pOadInfo->pTab->oad.value);
                return DATA_READ_WRITE_DENIED;
            }

            /* APP�����ն�����IP��ַ��Ч,д����������451004FF,451104FF��Ϊ���ж����� */
            NormalOad.oad.value = pOadInfo->pTab->oad.value + 0xFF;
            uint8 typeFlag = 1;
            db_write_nomal(pApduInfo->hUdp, &NormalOad, &typeFlag, 1);
            
            //ͨ���������ݱ���¼����ж��Ƿ�֪ͨsmiOS�޸�ip
            //memcpy(&data, buffer, srcLen);
            //set_tmn_ip(0, data);
        }
        break;
        case 0x05:
        { 
//            OOP_ETHMAC_T ETHMAC_T;
//            srcLen=sizeof(OOP_ETHMAC_T);
//            uint8 buffer[srcLen];
//            result=buf_to_data_eth_config((uint8 *)inData, inLen, &offset, buffer, &len);
//            if(result !=ERR_OK)
//            {
//                return result;
//            }
//
//            NormalOad.classtag = CLASS_DATA_UNINIT;
//            ret = db_write_nomal(pApduInfo->hUdp, &NormalOad,buffer,len);
//            if(ret < 0)
//            {
//                PRTL_FMT_DEBUG("class26_invoke_set db_write_nomal err[%d], oad[0x%08x]\n", 
//                    ret, pOadInfo->pTab->oad.value);
//                return DATA_READ_WRITE_DENIED;
//            }        
        }
        break;
        default:

        break;
            
    }
        return result;
}

/**
*********************************************************************
* @name��       class26_invoke_get
* @brief��      class26��GET����
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
OOP_DAR_E class26_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{

    int ret;
    uint32 len;
    uint32 offset=0;
    uint32 srcLen=0;
    int result;
    NOMAL_OAD_T NormalOad ;

    MEMZERO(&NormalOad, sizeof(NormalOad));
    NormalOad.logicId = 0;
    NormalOad.infoNum = 0;
    NormalOad.oad.value = pOadInfo->pTab->oad.value;
    
    switch(pOadInfo->pTab->oad.value)
    {
        case 0x45100200:
        case 0x45110200:
        { 
            OOP_ETHCONFIG_T data;
            
            memset((uint8*)&data,0x00,sizeof(OOP_ETHCONFIG_T));
            ret = db_read_nomal(pApduInfo->hUdp, &NormalOad,sizeof(OOP_ETHCONFIG_T), (uint8*)&data, &len);
            if ((ret != 0)||(len != sizeof(OOP_ETHCONFIG_T)))
            {
                PRTL_FMT_DEBUG("class26_invoke_get db_read_nomal err[%d], oad[0x%08x]\n", 
                    ret, pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4] = 0x01;
                outData->pResultData[5] = 0x00;
                outData->resultDataLen = 6;
                return ERR_OK;
            }

            printf("class26_invoke_get len is %d\n",len);
            PRTL_FMT_LOGCC("class26_invoke_get data.workmode = %d\n", data.workmode);
            PRTL_FMT_LOGCC("class26_invoke_get data.contype = %d\n", data.contype);
            PRTL_FMT_LOGCC("class26_invoke_get data.conmode = %d\n", data.conmode);
            PRTL_FMT_LOGCC("class26_invoke_get data.listenNum = %d\n", data.listenNum);
            uint8 i = 0;
            for (i = 0; i < OOP_MAX_LISTENS; i++)
            {
                PRTL_FMT_LOGCC("class26_invoke_get data.listen[%d] = %d\n", data.listen[i]);
            }
            PRTL_BUF_LOGCC(data.proxyip, 4, "class26_invoke_get data.proxyip:");
            PRTL_FMT_LOGCC("class26_invoke_get data.proxyport = %d\n", data.proxyport);
            PRTL_FMT_LOGCC("class26_invoke_get data.dail.body = %d\n", data.dail.body);
            PRTL_FMT_LOGCC("class26_invoke_get data.heart = %d\n", data.heart);
            if(data.conmode > 1)
            {
                data.conmode = 0;
            }
            result = data_to_buf_eth_config(outData->pResultData+5, outDataMax, &offset, (uint8*)&data, len);
            if (result == ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = offset + 5;
            }
            else
            {
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->pResultData[5] = 0x00;
                outData->resultDataLen = 6;
            }
        }
        break;
        case 0x45100300:
        case 0x45110300:
        { 
            srcLen = sizeof(OOP_MASTERPARAMS_T);
            uint8 buffer[srcLen];
            memset(buffer, 0x00, sizeof(OOP_MASTERPARAMS_T));
            ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, srcLen, buffer, &len);
            if((ret != 0)||(len !=sizeof(OOP_MASTERPARAMS_T)))
            {
                PRTL_FMT_DEBUG("class26_invoke_get db_read_nomal err[%d], oad[0x%08x]\n", 
                    ret, pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4]=0x01;
                outData->pResultData[5]=0x00;
                outData->resultDataLen=6;
                return ERR_OK;
            }
            printf("class26_invoke_get len is %d\n",len);
            result = data_to_buf_eth_master_commu_param(outData->pResultData+5, outDataMax, &offset, buffer,len);
            if (result == ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = offset + 5;
            }
            else
            {
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->pResultData[5] = 0x00;
                outData->resultDataLen = 6;
            }            
        }
        break;
        case 0x45100400:
        case 0x45110400:
        { 
            srcLen = sizeof(OOP_ETHIP_T);
            uint8 buffer[srcLen];
            memset(buffer, 0x00, sizeof(OOP_ETHIP_T));
            ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, srcLen, buffer, &len);
            if ((ret != 0)||(len != sizeof(OOP_ETHIP_T)))
            {
                PRTL_FMT_DEBUG("class26_invoke_get db_read_nomal err[%d], oad[0x%08x]\n", 
                    ret, pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->pResultData[5] = 00;
                outData->resultDataLen = 6;
                return DATA_SUCCESS;
            }
            result = data_to_buf_eth_net_config(outData->pResultData+5, outDataMax, &offset, buffer,len);
            if(result == ERR_OK)
            {
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = offset + 5;
            }
            else
            {
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->pResultData[5] = 0x00;
                outData->resultDataLen = 6;
            }
        }
        break;
        case 0x45100500:
        case 0x45110500:
        { 
            srcLen = sizeof(OOP_ETHMAC_T);
            uint8  buffer[srcLen];
            memset(buffer, 0x00, sizeof(OOP_ETHMAC_T));
            ret = db_read_nomal(pApduInfo->hUdp, &NormalOad, srcLen, buffer, &len);
            if ((ret != 0)||(len != sizeof(OOP_ETHMAC_T)))
            {
                PRTL_FMT_DEBUG("class26_invoke_get db_read_nomal err[%d], oad[0x%08x]\n", 
                    ret, pOadInfo->pTab->oad.value);
                memcpy_r(outData->pResultData, (uint8*)&pOadInfo->pTab->oad.value, 4);
                outData->pResultData[4] = 0x01;
                outData->pResultData[5] = 0x00;
                outData->resultDataLen = 6;
                return ERR_OK;
            }
            result = data_to_buf_eth_mac_addr(outData->pResultData+5, outDataMax, &offset, buffer,len);
            if (result == ERR_OK)
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4] = 0x01;
                outData->resultDataLen = offset + 5;
            }
            else
            {
                memcpy_r(outData->pResultData,(uint8*)&pOadInfo->pTab->oad.value,4);
                outData->pResultData[4] = 0x01;
                outData->pResultData[5] = 0x00;
                outData->resultDataLen = 6;
            }

        }
        break;
        default:

        break;
            
    }
    return result;
}

/**
*********************************************************************
* @name��       class26_invoke_act
* @brief��      class26��ACTION����
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
OOP_DAR_E class26_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    uint32 offset  = 0;
    
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x45100100:
        case 0x45110100:
        {
            /* ��֧�ֹ�Լ ��̫��������Ч��������� */
            NOMAL_OAD_T tOAD;
            uint8       typeFlag = 1;

            /* д451004FE */
            memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
            tOAD.oad.value = pOadInfo->pTab->oad.value + 0x03FE;
            tOAD.classtag = CLASS_DATA_UNINIT;
            db_write_nomal(pApduInfo->hUdp, &tOAD, &typeFlag, 1);
            tOAD.oad.value += 1;
            db_write_nomal(pApduInfo->hUdp, &tOAD, &typeFlag, 1);
            
            memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));  //OMD
            offset += sizeof(OOP_OAD_U);
            outData->pResultData[offset ++] = 0x00;                            //DAR type
            outData->pResultData[offset ++] = 0x00;                            //DATA:NULL
            outData->resultDataLen = offset;
            break;
        }  
        default:  return DATA_OBJECT_UNAVAILABLE;
    }
    
    return DATA_SUCCESS;
}


#endif
