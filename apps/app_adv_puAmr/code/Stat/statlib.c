/*
*********************************************************************
* @file    AutoTask.c
* @brief�� ͳ��ģ��ʵ���ļ�
* @author������
* @date��   2019.11.11
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/

#include "framecomm.h"
#include "storage.h"
#include "db_api.h"
#include "statmain.h"
#include "statlib.h"
#include "oopVarUpdate.h"
#include "msg_id.h"
extern uint8  g_udisk_status;
/* ͨ�ž����һ���߳�ȫ��Ψһ*/
extern DB_CLIENT g_staUdpHandle;
extern uint8   g_MinuteCalcEn;

extern OOP_DATETIME_S_T   realstattime;     /*ϵͳʵʱʱ��,ȫ��ʱ�����*/
extern OOP_DATETIME_S_T   laststattime;     /*ϵͳ��һ��ͳ��ʱ��*/

/*����Ϊʱ�����,��ʱ,����,���±��*/
extern uint8    pastminflag;
extern uint8    pasthourflag;
extern uint8    pastdayflag;
extern uint8    pastmonflag;

//����ʵʱ����
extern STA_ACDATA_T g_acData;

extern OOP_METER_P_LIST_T oopMeterList;

extern STA_PORT_AUTHORIZE_INFO g_portRS232;
extern STA_PORT_AUTHORIZE_INFO g_portRS485;
extern STA_PORT_AUTHORIZE_INFO g_portUSB;
extern STA_PORT_AUTHORIZE_INFO g_portIR;
extern STA_PORT_AUTHORIZE_INFO g_portSSH;
extern STA_PORT_AUTHORIZE_INFO g_portSFTP;
extern void CheckERC_3140(int fd, uint32 msg_id, uint8 *pmsg, uint16 msglen);
//�ܼ���洢��ţ������
uint32 g_tgStoreNo = 0;
STA_TGDEL_T g_tgdel = {0};

extern uint8 app_get_localaddr(uint8 **ppAddr);
extern uint8 pulse_get_ok();

STA_SLIPPOWER_FIFO_T g_slippower_fifo[OOP_MAX_TGSETS] = {{0}}; //����ʻ������
extern OOP_SERVICES_T g_serData;
#if DESC("��������ʵ��",1)

/**
*********************************************************************
* @brief��      long unsigned���������ֵ
* @param[in]�� bItemNum  ��������
* @param[in]�� awSrc      ��ֵ������
* @return��     ������
*********************************************************************
*/
uint16 sta_lmax_calc(uint8 bItemNum, uint16 *awSrc)
{
    uint8 i;
    uint16  wMax=0;
    
    for(i=0; i<bItemNum; i++)
        wMax = ((awSrc[i]>wMax) ? awSrc[i]:wMax);
    return wMax;
}

/**
*********************************************************************
* @brief��      long unsigned��������Сֵ
* @param[in]�� bItemNum  ��������
* @param[in]�� awSrc      ��ֵ������
* @return��     ������
*********************************************************************
*/
uint16 sta_lmin_calc(uint8 bItemNum, uint16 *awSrc)
{
    uint8 i=0;
    uint16  wMin=awSrc[i];
    
    for(i=1; i<bItemNum; i++)
        wMin = ((awSrc[i]>wMin) ? wMin:awSrc[i]);
    return wMin;
}

/**
*********************************************************************
* @brief��      double long unsigned���������ֵ
* @param[in]�� bItemNum  ��������
* @param[in]�� awSrc      ��ֵ������
* @return��     ������
*********************************************************************
*/
uint32 sta_dlmax_calc(uint8 bItemNum, uint32 *awSrc)
{
    uint8 i;
    uint32  wMax=0;
    
    for(i=0; i<bItemNum; i++)
        wMax = ((awSrc[i]>wMax) ? awSrc[i]:wMax);
    return wMax;
}

/**
*********************************************************************
* @brief��      double long unsigned��������Сֵ
* @param[in]�� bItemNum  ��������
* @param[in]�� awSrc      ��ֵ������
* @return��     ������
*********************************************************************
*/
uint32 sta_dlmin_calc(uint8 bItemNum, uint32 *awSrc)
{
    uint8 i=0;
    uint32  wMin=awSrc[i];
    
    for(i=1; i<bItemNum; i++)
        wMin = ((awSrc[i]>wMin) ? wMin:awSrc[i]);
    return wMin;
}


/**
*********************************************************************
* @brief��      ������ŵ�ԭ��ľ���ֵ
* @param[in]�� dec  ��ֵ����
* @param[out]��NA
* @return��     ������
*********************************************************************
*/
int sta_signed_abs_calc(int dec)
{
    int value;
    value = (dec&0x7fffffff);
    return value;
}

/**
*********************************************************************
* @brief��      ��ȡ�ն˻򽻲�ͨ�ŵ�ַ
* @param[in]:  pstTSA1 ����Ƚϵ�ͨ�ŵ�ַ1
* @param[in]:  pstTSA2 ����Ƚϵ�ͨ�ŵ�ַ2
* @return��     FALSE-����ȣ�TRUE-���
*********************************************************************
*/
BOOL sta_tsa_equal(OOP_TSA_T *pstTSA1, OOP_TSA_T *pstTSA2)
{
    uint32 i = 0;
    
    if(pstTSA1->af != pstTSA2->af)
    {
        return FALSE;
    }

    for(i = 0; i < pstTSA1->len+1; i++)
    {
        if(pstTSA1->add[i] != pstTSA2->add[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

/**
*********************************************************************
* @brief��      ��ȡ�ն˻򽻲�ͨ�ŵ�ַ
* @param[out]: pstTSA ͨ�ŵ�ַ
* @return��     void
*********************************************************************
*/
int sta_addr_read(uint8     logicId, OOP_OAD_U unOAD, void *buffer, uint8 maxLen)
{
    NOMAL_OAD_T nomalOAD = {0};    
    uint32 len = 0;

    nomalOAD.infoNum = 0;
    nomalOAD.logicId = logicId;
    nomalOAD.oad.value = unOAD.value;

    return db_read_nomal(g_staUdpHandle, &nomalOAD, maxLen, (uint8*)buffer, &len);       
}


/**
*********************************************************************
* @brief��      ͨ��ͨ�ŵ�ַ��ȡ��������Ϣ
* @param[in]:  pstTSA ͨ�ŵ�ַ
* @param[out]: pstMtrInfo ��������Ϣ
* @return��     void
*********************************************************************
*/
int sta_mtrtype_read(OOP_TSA_T *pstTSA, STA_MTR_INFO_T* pstMtrInfo)
{
    OOP_OAD_U oad;
    OOP_METER_T meter;
    int i = 0;
    uint8 *pAddr = NULL;
    uint8 len = 0; 
    OOP_OCTETVAR16_T pusleAddr = {0};
    uint8 revValue[16] = {0};

    //����8����������㣬�ж��ܼ������Ƿ�Ϊ����
    oad.value = 0x24010200;
    for(i = 0; i < 8; i++)
    {
        oad.nObjID = 0x2401 + i;
        
        if(sta_addr_read(0, oad, &pusleAddr, sizeof(pusleAddr)) == 0)
        {
            memcpy(revValue, pusleAddr.value, pusleAddr.nNum);
            if(memcmp(revValue, pstTSA->add, 6) == 0)
            {
                pstMtrInfo->type = MTR_PULSE;
                pstMtrInfo->pulseOAD.value = oad.value;
                return 0;
            }
        }        
    }

    //����485�����㣬�ж��ܼ������Ƿ�Ϊ485��
    for(i = 0; i < oopMeterList.oopMeterNum; i++)
    {
        if(app_get_meter_list_by_id(i, &meter) == 0)
        {
            if(sta_tsa_equal(&meter.basic.tsa, pstTSA))
            {
                if(meter.basic.port.nObjID == 0xf201)
                {
                    pstMtrInfo->type = MTR_485;
                    pstMtrInfo->nIndex = meter.nIndex;
                    memcpy(&pstMtrInfo->tsa, pstTSA, sizeof(OOP_TSA_T));
					return 0;
                }
            }
        }        
    }

    //�ն˵�ַ��֮��ȣ�˵���ò�����Ϊ����
    len = app_get_localaddr(&pAddr);
    if(len > 0)
    {
        memcpy_rev(revValue, pAddr, len);
        
        if(memcmp(revValue, pstTSA->add, 6) == 0)
        {
            pstMtrInfo->type = MTR_AC;
            return 0;
        }
    }

    return -1;
}

/**
*********************************************************************
* @brief��      ��ȡ�ն˻򽻲�ͨ�ŵ�ַ
* @param[out]: pstTSA ͨ�ŵ�ַ
* @return��     void
*********************************************************************
*/
void sta_tsa_print(OOP_TSA_T *pstTSA, int (*func)(const char *,...))
{
    int i = 0;

    //+1Ϊ��ַ���ȣ�+2����ͷ
    for(i = 0; i < pstTSA->len+2; i++)
    {
        func("%02x ",*((uint8*)pstTSA+i));
    }

    func("\n");

    return;    
}

/*******************************************************************************
* ��������: sta_checksum_calc
* ��������: �����ۼӺ�
* �������: DataLen  ���ݳ���
*           SrcData  ���ݻ�����
* �������: ��
* �� �� ֵ: �ۼӺ�
*******************************************************************************/
uint8 sta_checksum_calc(uint8 *SrcData, int DataLen)
{
    int     i  = 0;
    uint8   CS = 0;

    for (i = 0; i < DataLen; i++)
    {
        CS += SrcData[i];
    }

    CS += 0x33;
    
    return CS;
}

/**
*********************************************************************
* @brief��      ɾ����¼��OAD����
* @param[in]�� clientid �������ľ��
* @param[in]:  pNormal  ��ͨOAD��Ϣ
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void 
*********************************************************************
*/
int sta_delete_record(DB_CLIENT clientid, RECORD_OAD_T *pRecord)
{
    int ret = 0;
    CLEAR_DATA_T delData;

    delData.bClassTag = FALSE;
    memcpy(&delData.recordOAD, pRecord, sizeof(RECORD_OAD_T));
    
    ret = db_clear_oad_data(clientid, &delData);
    if (ret != ERR_OK)
    {
        return DATA_READ_WRITE_DENIED;
    }

    return ret;
}

/**
*********************************************************************
* @brief��      ����������������
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void 
*********************************************************************
*/
void sta_pulse_pastday_data_clear(void)
{
    int ret = ERR_OK;
    uint32 i = 0;
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;

    MEMZERO(&energy, sizeof(energy));
    energy.nNum = 5;

    for(i = 0; i < 8; i++)
    {
        nomalOAD.oad.nObjID = 0x2401 + i;
    
        nomalOAD.oad.attID = 7;//���������й�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 9;//���շ����й�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 11;//���������޹�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day positive reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 13;//���շ����޹�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
    }
    
    return;
}

/**
*********************************************************************
* @brief��      ����������������
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void 
*********************************************************************
*/
void sta_pulse_pastmon_data_clear(void)
{
    int ret = ERR_OK;
    uint32 i = 0;
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;

    MEMZERO(&energy, sizeof(energy));
    energy.nNum = 5;

    for(i = 0; i < 8; i++)
    {
        nomalOAD.oad.nObjID = 0x2401 + i;
    
        nomalOAD.oad.attID = 8;//���������й�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("month positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 10;//���·����й�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("month negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 12;//���������޹�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("month positive reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 14;//���·����޹�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("month negative reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
        
            return;
        }     
    }
    
    return;
}

/**
*********************************************************************
* @brief��      �����ܼ�����������
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void 
*********************************************************************
*/
void sta_tgsets_pastday_data_clear(void)
{
    int ret = ERR_OK;
    uint32 i = 0;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HENERGYS_T henergys;

    MEMZERO(&henergys, sizeof(henergys));
    henergys.nNum = 5;

    for(i = 0; i < 8; i++)
    {
        nomalOAD.oad.nObjID = 0x2301 + i;
    
        nomalOAD.oad.attID = 7;//���й�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 8;//���޹�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }
    
    return;
}

/**
*********************************************************************
* @brief��      �����ܼ�����������
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void 
*********************************************************************
*/
void sta_tgsets_pastmon_data_clear(void)
{
    int ret = ERR_OK;
    uint32 i = 0;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HENERGYS_T henergys;

    MEMZERO(&henergys, sizeof(henergys));
    henergys.nNum = 5;

    for(i = 0; i < 8; i++)
    {
        nomalOAD.oad.nObjID = 0x2301 + i;
    
        nomalOAD.oad.attID = 9;//���й�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    
        nomalOAD.oad.attID = 10;//���޹�����
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }
    
    return;
}

/**********************************************************************
* @name      : is_total_del_finish
* @brief     ���ϴ��Ƿ������
* @param[in] ��NULL
* @return    ��TRUE-���� FALSE-û��
**********************************************************************/
BOOL is_total_del_finish(void)
{
    BOOL flag = FALSE;
    
    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_tgdel.flock);

    //����ģʽ
    if (g_tgdel.delVaild != 0)
    {
        flag = FALSE;
    }
    else
    {
        flag = TRUE;
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_tgdel.flock);
    
    return flag;
}

/**********************************************************************
* @name      : set_total_del_flag
* @brief     ������ɾ�����
* @param[in] ��flag ---- ����ɾ����ʶ
* @return    ��NULL
**********************************************************************/
void set_total_del_flag(uint8 delVaild)
{
//    int i = 0;
//    //�ȴ��ϴ�ɾ����Ǵ������
//    for (i = 0; i < 30; i++)
//    {
//        if (is_total_del_finish() == FALSE)
//        {
//            usleep(100000);
//        }
//        else
//        {
//            break;
//        }
//    }
//    if (i == 30)
//    {
//        PRTL_FMT_DEBUG("�ϴ��ܼ�����ɾ����Ǵ���ʱ\n");
//        return;
//    }
    
    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_tgdel.flock);

    g_tgdel.delVaild |= delVaild;
    
    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_tgdel.flock);

    return;
}

/**********************************************************************
* @name      : get_total_del_flag
* @brief     ����ȡɾ�����ݱ�ʶ
* @param[in] ��NULL
* @return    ��flag ---- ����ɾ����ʶ
**********************************************************************/
uint8 get_total_del_flag(void)
{
    uint8 delVaild = 0;
    
    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_tgdel.flock);

    //����ģʽ
    delVaild = g_tgdel.delVaild;
    if (delVaild != 0)
    {
        g_tgdel.delVaild = 0;
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_tgdel.flock);
    
    return delVaild;
}

/**
*********************************************************************
* @brief��      ���չ��µ���������
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void 
*********************************************************************
*/
void sta_past_data_clear(void)
{
    int i = 0;
    OOP_OAD_U tgOAD = {0};
    uint8 tgcflg=0;

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("past day clear pulse and tgsets data.\n");
        sta_pulse_pastday_data_clear();
        sta_tgsets_pastday_data_clear();
    }

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("past mon clear pulse and tgsets data.\n");
        sta_pulse_pastmon_data_clear();
        sta_tgsets_pastmon_data_clear();
    }

    tgcflg = get_total_del_flag();

    //�ܼ������ñ������������
    for(i = 0; i < 8; i++)
    {
        if(tgcflg&(1UL<<i))
        {
            tgOAD.nObjID = 0x2301+i;
            sta_tg_data_clear(g_staUdpHandle, tgOAD);
        }
    }
}

#endif

#if DESC("�ն˶˿���Ȩ",1)

/**
*********************************************************************
* @brief��      SSH������Ȩ����
* @param[in]�� STA_PORT_AUTHORIZE_E status ������Ȩ״̬
* @return��     void
*********************************************************************
*/


/**
*********************************************************************
* @brief��      SSH������Ȩ����
* @param[in]�� STA_PORT_AUTHORIZE_E status ������Ȩ״̬
* @return��     void
*********************************************************************
*/
int sta_ssh_service_authorize_operate(STA_PORT_AUTHORIZE_E status)
{
    int ret = ERR_OK;
    uint8 msgdata[1400] = {0};
    uint16 msglen=0;

    if(status == STA_PORT_AUTH_FORBIDDEN)
    {
        msgdata[msglen++] = 1; //�ر�SSH��Ȩ
        AUTH_FMT_DEBUG("SSH service close.\n");
    }
    else
    {
        msgdata[msglen++] = 0; //����SSH��Ȩ
        AUTH_FMT_DEBUG("SSH service open.\n");
    }
    
    ret = appmain_send_msg(MSG_APPAMIN_SSH_PORT_AUTHORIZE, 0, msgdata, msglen);
    if(ret != ERR_OK)
    {
        AUTH_FMT_DEBUG("appmain_send_msg failed. ret(%d)\n",ret);
        return ret;
    }

    return ret;
}

/**
*********************************************************************
* @brief��      �ն˶˿���Ȩ״̬
* @param[in]�� NA
* @return��     void
*********************************************************************
*/
void sta_tmn_port_authorize_status(uint8 tmAuth, STA_PORT_AUTHORIZE_INFO *authInfo)
{
    authInfo->authTime = tmAuth;
    authInfo->status = AUTH_STATUS_GET(authInfo->authTime);
}

/**
*********************************************************************
* @brief��      �ն˶˿���Ȩ
* @param[in]�� NA
* @return��     void
*********************************************************************
*/
void sta_tmn_port_authorize_tmcnt(void)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomal = {0};
    OOP_PORTS_T data;
    OOP_SERVICES_T serData;

    MEMZERO(&data, sizeof(data));
    MEMZERO(&serData, sizeof(serData));
    memcpy(&serData, &g_serData, sizeof(serData));
    
    nomal.classtag = CLASS_DATA_UNINIT;

    //��Ȩʱ��Ϊ��������ÿ���ּ�ȥ1,ֻ����ʱ��Ȩ��ˢ�·�������������Ȩ�͹ر���Ȩ������Ҫ
    if((g_portSSH.authTime > 0) && (g_portSSH.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portSSH.authTime--;
    
        serData.item[0].times = g_portSSH.authTime;

        nomal.oad.value = 0x43000B00;
        //����д��
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&serData, sizeof(serData));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(serData));
            return;
        }
    } 
    
    if((g_portRS232.authTime > 0) && (g_portRS232.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portRS232.authTime--;
    
        data.item[0] = g_portRS232.authTime;

        nomal.oad.value = 0xF2000500;
        //����д��
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            return;
        }
    }

    if((g_portRS485.authTime > 0) && (g_portRS485.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portRS485.authTime--;
    
        data.item[0] = g_portRS485.authTime;

        nomal.oad.value = 0xF2010500;
        //����д��
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            return;
        }        
    }
        
    if((g_portIR.authTime > 0) && (g_portIR.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portIR.authTime--;
    
        data.item[0] = g_portIR.authTime;

        nomal.oad.value = 0xF2020500;
        //����д��
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            return;
        }       
    } 

    if((g_portUSB.authTime > 0) && (g_portUSB.status == STA_PORT_AUTH_PROVISIONAL))
    {
        g_portUSB.authTime--;
    
        data.item[0] = g_portUSB.authTime;


        nomal.oad.value = 0xF20E0500;
        //����д��
        ret = db_write_nomal(g_staUdpHandle, &nomal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            AUTH_FMT_DEBUG("tmn_port_authorize write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(data));
            return;
        }        
    }  

    return;
}

/**
*********************************************************************
* @brief��      �ն˶˿���Ȩ
* @param[in]�� NA
* @return��     void
*********************************************************************
*/
void sta_tmn_port_authorize_operate(void)
{
    int ret = ERR_OK;
    uint8 msgdata[1400] = {0};
    uint16 msglen=0;

    if(g_portRS232.isAuthChg == TRUE)
    {
        g_portRS232.isAuthChg = FALSE;
            
        if(g_portRS232.status == STA_PORT_AUTH_FORBIDDEN)
        {
            //TODO �ر���Ȩ
        }
        else
        {
            //TODO ������Ȩ
        }
    }

    if(g_portRS485.isAuthChg == TRUE)
    {
        g_portRS485.isAuthChg = FALSE;
    
        if(g_portRS485.status == STA_PORT_AUTH_FORBIDDEN)
        {
            //TODO �ر���Ȩ
        }
        else
        {
            //TODO ������Ȩ
        }        
    }

    if(g_portIR.isAuthChg == TRUE)
    {
        g_portIR.isAuthChg = FALSE;
            
        if(g_portIR.status == STA_PORT_AUTH_FORBIDDEN)
        {
            //TODO �ر���Ȩ
        }
        else
        {
            //TODO ������Ȩ
        }               
    }

    if(g_portUSB.isAuthChg == TRUE)
    {
        g_portUSB.isAuthChg = FALSE;
    
        //�ر���ȨʱU���ǲ���״̬��������������Ϣ
        if((g_portUSB.status == STA_PORT_AUTH_FORBIDDEN)&&(g_udisk_status == 1))
        {
            msglen = 0;
            msgdata[msglen++] = 2; 
            EVENT_FMT_DEBUG("USB disk exist.\n");
            CheckERC_3140(g_staUdpHandle, MSG_APPAMIN_UDISK_AUTHORIZE, msgdata, msglen);
        }
    }

    if(g_portSSH.isAuthChg == TRUE)
    {                
        if(g_portSSH.status == STA_PORT_AUTH_FORBIDDEN)
        {
            msglen = 0;
            msgdata[msglen++] = 1; //�ر�SSH��Ȩ
            
            AUTH_BUF_DEBUG(msgdata, msglen, "SSH service close.\n");

            ret = appmain_send_msg(MSG_APPAMIN_SERVICE_STOP, 0, msgdata, msglen);
            if(ret != ERR_OK)
            {
                AUTH_FMT_DEBUG("auth msg send failed. ret(%d)\n",ret);
                return;
            }
        }
//        else
//        {
//            msgdata[msglen++] = 0; //����SSH��Ȩ
//            AUTH_FMT_DEBUG("SSH service open.\n");           
//        }

        g_portSSH.isAuthChg = FALSE;
    }

    if(g_portSFTP.isAuthChg == TRUE)
    {                
        if(g_portSFTP.status == STA_PORT_AUTH_FORBIDDEN)
        {
            msglen = 0;
            msgdata[msglen++] = 2; //�ر�SSH��Ȩ
            
            AUTH_BUF_DEBUG(msgdata, msglen, "SFTP service close.\n");

            ret = appmain_send_msg(MSG_APPAMIN_SERVICE_STOP, 0, msgdata, msglen);
            if(ret != ERR_OK)
            {
                AUTH_FMT_DEBUG("auth msg send failed. ret(%d)\n",ret);
                return;
            }
        }
//        else
//        {
//            msgdata[msglen++] = 0; //����SSH��Ȩ
//            AUTH_FMT_DEBUG("SSH service open.\n");           
//        }

        g_portSFTP.isAuthChg = FALSE;
    }
}

#endif

#if DESC("��ѹ�ϸ��ʼ���",1)

/**
*********************************************************************
* @brief��      ��������ͳ�Ƶ�ʱ���,���Ϊ�����ѹ�ϸ���ʹ�ã�����ͳ��ͬʱ����
* @param[out]��alterTime ����ͳ�Ƶ�ʱ��������, 2��Ԫ�أ�0-�գ�1-��
* @return��     void
*********************************************************************
*/
void sta_volt_monitor_time(int *alterTime)
{
    time_t tmpTime = 0;
    time_t tmpLstTime = 0;
    int  addTime = 0;     // �仯ʱ�� 
    int  detaTime[2] = {0}; // 2��Ԫ�أ�0-�գ�1-��,�������ʱ���㲻ͬ�����ܺ�һ
    static BOOLEAN firstRun = 1;

    //����������ramdisk��������ȫ�ֻ�static�����ȼ��滻
    DT_DateTimeShort2Time(&realstattime, &tmpTime);
    DT_DateTimeShort2Time(&laststattime, &tmpLstTime);

    //��������ͳ�Ƽ���ķ�����,�ϴ�ʱ����Ե�����������
    addTime = (int)(tmpTime -(tmpLstTime/60)*60)/60;

    // ��λ�϶�����1��������ɣ����ն˸�λ��������ܶ�ʧ�ϸ�ʱ��
    if(firstRun)
    {
        if (addTime > 1)
        {
            return;
        }

        firstRun = 0;
    }

    STAT_FMT_DEBUG("volt monitor curTime(%d) - lstTime(%d) = (%d)s ~ (%d)min.\n", tmpTime, tmpLstTime, tmpTime-tmpLstTime, addTime);

    /* ���ض�ʱ��һ�β�����ʱ�� */
    if (addTime < 0)
    {
        return;
    }

    /*���洦������պ���ʱ��һ��*/
    detaTime[STA_CALC_DAY] = addTime;
    detaTime[STA_CALC_MON] = addTime;

    /* ������ ��0ʱ0�ֵĹ��գ���ʱ����*/
    if ((pastdayflag) && ((realstattime.hour != 0x00) || (realstattime.minute != 0x00)))
    {
        STAT_FMT_DEBUG("past day by datetime update.\n");
        // ��������һ��,���˿̵�Խ��ʱ��ȫ��
        detaTime[STA_CALC_DAY] = realstattime.hour * 60 + realstattime.minute;
    }

    /* ������ ��1��0ʱ0�ֵĹ��£���ʱ����*/
    if ((pastmonflag) && ((realstattime.day != 0x01) || (realstattime.hour != 0x00) || (realstattime.minute != 0x00)))
    {
        STAT_FMT_DEBUG("past month by datetime update.\n");
        // ��������һ��,���˿̵�Խ��ʱ��ȫ��
        detaTime[STA_CALC_MON] = (realstattime.day - 1) * 1440 + realstattime.hour * 60 + realstattime.minute;
    }

    memcpy(alterTime, detaTime, sizeof(detaTime));
}

/**
*********************************************************************
* @brief��      ��ȡ��ѹԽ�޲���
* @param[out]��olimTime Խ��ʱ������
* @param[out]��qTime �ϸ�ʱ������
* @return��     void
*********************************************************************
*/
int sta_volt_param_read(OOP_VOLTPARAM_T *param)
{
    int ret = 0;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;
    uint32 inMaxLen = sizeof(OOP_VOLTPARAM_T);
    OOP_OAD_U voltpOAD = {0x40300200};

    nomalOAD.logicId = 0;
    memcpy(&nomalOAD.oad, &voltpOAD, sizeof(OOP_OAD_U));

    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, inMaxLen, (uint8*)param, &len);
    if(ret != 0 )
    {
        STAT_FMT_DEBUG("volt limit paramter read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, inMaxLen);
        return ret;
    }

    return ret;
}

/**
*********************************************************************
* @brief��      �����ѹԽ����ʱ���ۻ�ֵ
* @param[in]�� voltAccCfg �ۼ����ò���
* @param[out]��accTime  Խ���ۻ�ʱ������
* @param[out]��isQuality �Ƿ�ϸ�
* @return��     void
*********************************************************************
*/
void sta_volt_uptime_calc(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};
    OOP_WORD3V_T phaseVolt;   //�����ѹ
    OOP_VOLTPARAM_T voltParam; //��ѹ�ϸ��ʲ���
    uint16 addTime;            //���˴�ͳ��ʱ�ۻ�ʱ��  
    uint8 upflag = 1;
    uint8 upupflag = 1;

    phaseVolt = voltAccCfg.phaseVolt;
    voltParam = voltAccCfg.voltParam;
    addTime = voltAccCfg.addTime;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    //TODO �ж�Խ�޲�������Ч�ԣ��ݴ�����upflag��upupflag,����voltAccCfg�д���

    //�����ѹԽ�����ۻ�ʱ��   
    if(upflag && !upupflag) 
    {
        if(volt[i] >= voltParam.upVolt)
        {
            acctime[i] += addTime;
            *isQuality = 0;
        }

    }
    else if(upflag && upupflag)                 
    {
        if(volt[i] >= voltParam.upVolt && volt[i] <voltParam.uupVolt) 
        {
            acctime[i] += addTime;
            *isQuality = 0;
        }
    }

    memcpy(accTime, acctime, sizeof(acctime));    
}

/**
*********************************************************************
* @brief��      �����ѹԽ������ʱ���ۻ�ֵ
* @param[in]�� voltAccCfg �ۼ����ò���
* @param[out]��accTime  Խ���ۻ�ʱ������
* @param[out]��isQuality �Ƿ�ϸ�
* @return��     void
*********************************************************************
*/
void sta_volt_uuptime_calc(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};
    OOP_WORD3V_T phaseVolt;   //�����ѹ
    OOP_VOLTPARAM_T voltParam; //��ѹ�ϸ��ʲ���
    uint16 addTime;            //���˴�ͳ��ʱ�ۻ�ʱ��  
    uint8 upupflag = 1;
    static  uint16 upupTime[3] = {0};  //Խ�������ۼ�ʱ��
    uint8   upupLmtTime = 0;        //�����޳���ʱ�����
    uint16  upupRate = 0;           //�����޷���ϵ��

    phaseVolt = voltAccCfg.phaseVolt;
    voltParam = voltAccCfg.voltParam;
    addTime = voltAccCfg.addTime;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    //TODO �ж�Խ�޲�������Ч�ԣ��ݴ�����upflag��upupflag,����voltAccCfg�д���

    /* �����ѹԽ�������ۻ�ʱ�� TODO (1-upupRate)���㲻�ԣ���������
       upupRateҪ��Ĭ��ֵ��Ĭ��ֵ����ô�������俼���ú�ɡ� 
    */
    if(upupflag)
    {
        if(upupTime[i] > 0)      //������Խ������
        {
            if(volt[i] > (voltParam.uupVolt * (1-upupRate))) //û��С�ڻָ�ֵ��ʱ�ϸ�ʱ��Ҳ����
            {
                *isQuality = 0;
            }
        }
    
        if(volt[i] >= voltParam.uupVolt)
        {
            if(voltAccCfg.interval == STA_CALC_DAY)//ֻдһ���յ�,��ֹ��ͳ��ʱ��дһ��
            {
                upupTime[i] += addTime;
            }
    
            if(upupTime[i] >= upupLmtTime)
            {
            #if UPUP_EN == 1
                acctime[i] += addTime;
            #endif
                acctime[i+6] += addTime;
            }
            *isQuality = 0;
        }
        else
        {
            upupTime[i] = 0;
        }
    }

    memcpy(accTime, acctime, sizeof(acctime));    
}

/**
*********************************************************************
* @brief��      �����ѹԽ����ʱ���ۻ�ֵ
* @param[in]�� voltAccCfg �ۼ����ò���
* @param[out]��accTime  Խ���ۻ�ʱ������
* @param[out]��isQuality �Ƿ�ϸ�
* @return��     void
*********************************************************************
*/
void sta_volt_lowtime_calc(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};
    OOP_WORD3V_T phaseVolt;   //�����ѹ
    OOP_VOLTPARAM_T voltParam; //��ѹ�ϸ��ʲ���
    uint16 addTime;            //���˴�ͳ��ʱ�ۻ�ʱ��  
    uint8 dwflag = 1;
    uint8 dwdwflag = 1;

    phaseVolt = voltAccCfg.phaseVolt;
    voltParam = voltAccCfg.voltParam;
    addTime = voltAccCfg.addTime;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    //TODO �ж�Խ�޲�������Ч�ԣ��ݴ�����dwflag��dwdwflag,����voltAccCfg�д���

    //�����ѹԽ�����ۻ�ʱ��
    if(dwflag && !dwdwflag) //��ͳ��������,ֻͳ������
    {
        if(volt[i] <= voltParam.ldVolt)
        {
            acctime[i+3] += addTime;
            *isQuality = 0;
        }
    }
    else if(dwflag && dwdwflag)
    {
        if(volt[i] <= voltParam.ldVolt && volt[i] > voltParam.lldVolt) //С�����޴���������
        {
            acctime[i+3] += addTime;
            *isQuality = 0;
        }
    }

    memcpy(accTime, acctime, sizeof(acctime));    
}

/**
*********************************************************************
* @brief��      �����ѹԽ������ʱ���ۻ�ֵ
* @param[in]�� voltAccCfg �ۼ����ò���
* @param[out]��accTime  Խ���ۻ�ʱ������
* @param[out]��isQuality �Ƿ�ϸ�
* @return��     void
*********************************************************************
*/
void sta_volt_llowtime_calc(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};
    OOP_WORD3V_T phaseVolt;   //�����ѹ
    OOP_VOLTPARAM_T voltParam; //��ѹ�ϸ��ʲ���
    uint16 addTime;            //���˴�ͳ��ʱ�ۻ�ʱ��  
    uint8 dwdwflag = 1;
    static  uint16 dwdwTime[3] = {0};  //Խ�������ۼ�ʱ��
    uint8   dwdwLmtTime = 0;        //�����޳���ʱ�����
    uint16  dwdwRate = 0;           //�����޷���ϵ��

    phaseVolt = voltAccCfg.phaseVolt;
    voltParam = voltAccCfg.voltParam;
    addTime = voltAccCfg.addTime;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    //TODO �ж�Խ�޲�������Ч�ԣ��ݴ�����upflag��upupflag,����voltAccCfg�д���

    /* �����ѹԽ�������ۻ�ʱ�� TODO (1-upupRate)���㲻�ԣ���������
       upupRateҪ��Ĭ��ֵ��Ĭ��ֵ����ô�������俼���ú�ɡ� */
    if(dwdwflag)
    {
        if(dwdwTime[i] > 0)      //������Խ������
        {
            if(volt[i] < (voltParam.lldVolt * (1+dwdwRate))) //û�д��������޻ָ�ֵ��ʱ�ϸ�ʱ��Ҳ����
            {
                *isQuality = 0;
            }
        }
    
        if(volt[i] <= voltParam.lldVolt)
        {
            if(voltAccCfg.interval == STA_CALC_DAY)//ֻдһ���յ�,��ֹ��ͳ��ʱ��дһ��
            {
                dwdwTime[i] += addTime;
            }
    
            if(dwdwTime[i] >= dwdwLmtTime)
            {
            #if UPUP_EN == 1
                acctime[i+3] += addTime;
            #endif
                acctime[i+9] += addTime;
            }
            *isQuality = 0;
        }
        else
        {
            dwdwTime[i] = 0;
        }
    }

    memcpy(accTime, acctime, sizeof(acctime));    
}

/**
*********************************************************************
* @brief��      �ж�Խ�޲�����Խ��ʱ���ۻ�ֵ
* @param[in]�� voltAccCfg �ۼ����ò���
* @param[out]��accTime  Խ���ۻ�ʱ������
* @param[out]��isQuality �Ƿ�ϸ�
* @return��     void
*********************************************************************
*/
void sta_volt_overlim_judge(STA_VOLT_ACCTIME_CFG_T         voltAccCfg, uint32 i, uint16 *accTime, uint8 *isQuality)
{
  //TODO �ж�Խ�޲�������Ч�ԣ���voltAccCfg�д���

   //�����������ж�
   sta_volt_uuptime_calc(voltAccCfg, i, accTime, isQuality);

   //������δԽ�޲��������ж�
   if(*isQuality)
   {      
        sta_volt_uptime_calc(voltAccCfg, i, accTime, isQuality);

        //����δԽ�޲��������ж�
        if(*isQuality)
        {
            sta_volt_lowtime_calc(voltAccCfg, i, accTime, isQuality);
        
            //����δԽ�޲����������ж�
            if(*isQuality)
            {
                sta_volt_llowtime_calc(voltAccCfg, i, accTime, isQuality);   
            }
        }
   }

   return;
}

/**
*********************************************************************
* @brief��      ��ѹԽ�޺ͺϸ�ʱ���ۼ�
* @param[in]�� voltAccCfg �ۼ����ò���
* @param[out]��accTime  Խ���ۻ�ʱ������
* @param[out]��quaTime �ϸ��ۻ�ʱ������
* @return��     void
*********************************************************************
*/
void sta_volt_quaolimtime_acc(STA_VOLT_ACCTIME_CFG_T voltAccCfg, uint16 *accTime, uint16 *quaTime)
{
    uint16 volt[3] = {0};
    uint16 acctime[12] = {0};//����(A,B,C),����(A,B,C),������(A,B,C),������(A,B,C)
    uint32 i;
    uint8 isQuality;
    OOP_WORD3V_T phaseVolt;   //�����ѹ
    uint16 addTime;            //���˴�ͳ��ʱ�ۻ�ʱ�� 
    uint8 powerType = 0;

    phaseVolt = voltAccCfg.phaseVolt;
    addTime = voltAccCfg.addTime;
    powerType = voltAccCfg.powerType;
    
    memcpy(volt, phaseVolt.nValue, phaseVolt.nNum*sizeof(uint16));
    memcpy(acctime, accTime, sizeof(acctime));

    STAT_FMT_DEBUG("calcType(%d) addTime(%d), voltParam value: up(%d), upup(%d), dw(%d), dwdw(%d)\n", 
        voltAccCfg.interval, voltAccCfg.addTime, voltAccCfg.voltParam.upVolt, voltAccCfg.voltParam.uupVolt, voltAccCfg.voltParam.ldVolt, voltAccCfg.voltParam.lldVolt);

    for(i = 0; i < 3; i++)
    {
        STAT_FMT_DEBUG("volt[%d] over limit time last: qualify(%d), up(%d), dw(%d), upup(%d), dwdw(%d)\n", 
            i, quaTime[i], acctime[i], acctime[i+3], acctime[i+6], acctime[i+9]);

        STAT_FMT_DEBUG("volt[%d] rt value(%d)\n", i, volt[i]); 
    }
    
    if(volt[0] == 0 && volt[1] == 0 && volt[2] == 0)//TODO ͣ�粻��
    {
        return;
    }

    for(i = 0; i < 12; i++)
    {
        if(acctime[i] == NONEWORD)
        {
            acctime[i] = 0;
        }
    }

    for(i = 0; i < 3; i++)
    {
        //�������ߣ�B�಻ͳ��
        if((i == 1) && (powerType == PWRTYPE_3PHASE3)) continue;
        
        if(volt[i] == NONEWORD) continue;

        //Ĭ�ϵ�ѹ�ϸ�
        isQuality = 1;

        sta_volt_overlim_judge(voltAccCfg, i, acctime, &isQuality);

        //�����жϺ�û���κ�Խ�ޣ������ʱ���ѹ�ϸ�
        if(isQuality == 1)
        {
            quaTime[i]  += addTime;
        }

        // �������ݴ��������д�����ԣ�
        if (voltAccCfg.interval == STA_CALC_DAY)
        {
            if (acctime[i+0] > 1440)
                acctime[i+0] = 1440;
            if (acctime[i+3] > 1440)
                acctime[i+3] = 1440;
            if (acctime[i+6] > 1440)
                acctime[i+6] = 1440;
            if (acctime[i+9] > 1440)
                acctime[i+9] = 1440;
        }
    }

    //�������ߣ�B��ֱ�Ӻϸ�
    if (powerType == PWRTYPE_3PHASE3)
    {
        quaTime[1] += addTime;
    }

    memcpy(accTime, acctime, sizeof(acctime));

    return;
}

/**
*********************************************************************
* @brief��      �յ�ѹ�ϸ��ʼ����ݴ�
* @param[in]�� voltAccCfg �ۼ����ò���
* @param[in]�� accTime  Խ���ۻ�ʱ������
* @param[out]��quaTime   �ϸ��ۼ�ʱ������
* @return��     void
*********************************************************************
*/
void sta_volt_fault_tolerant(const uint16 *accTime, uint16 *quaTime)
{
    uint16 acctime[12] = {0};
    uint16 extTime[3] = {0};
    uint8 i = 0;

    memcpy(acctime, accTime, sizeof(acctime));
    memcpy(extTime, quaTime, sizeof(extTime));

    for(i=0;i<3;i++)// 2013 COMMON BUG 
    {
        /*1440������24Сʱ*/
        if(extTime[i] > 1440)  
        {
            extTime[i] = 1440;  
        }
    }

    if(pastdayflag)
    {
        #if UPUP_EN == 1
            if((acctime[0]+acctime[3])<1440)
                extTime[0] = 1440-(acctime[0]+acctime[3]);
            else
                extTime[0] = 0;
        #else
            if((acctime[0]+acctime[3]+acctime[6]+acctime[9])<1440)
                extTime[0] = 1440-(acctime[0]+acctime[3]+acctime[6]+acctime[9]);
            else
                extTime[0] = 0;
        #endif

        #if UPUP_EN == 1
            if((acctime[1]+acctime[4])<1440)
                extTime[1] = 1440-(acctime[1]+acctime[4]);
            else
                extTime[1] = 0;
        #else
            if((acctime[1]+acctime[4]+acctime[7]+acctime[10])<1440)
                extTime[1] = 1440-(acctime[1]+acctime[4]+acctime[7]+acctime[10]);
            else
                extTime[1] = 0;
        #endif

        #if UPUP_EN == 1
            if((acctime[2]+acctime[5])<1440)
                extTime[2] = 1440-(acctime[2]+acctime[5]);
            else
                extTime[2] = 0;
        #else
            if((acctime[2]+acctime[5]+acctime[8]+acctime[11])<1440)
                extTime[2] = 1440-(acctime[2]+acctime[5]+acctime[8]+acctime[11]);
            else
                extTime[2] = 0;
        #endif                
    }    
    
    memcpy(quaTime, extTime, sizeof(extTime));
}

/**
*********************************************************************
* @brief��      ͳ�Ƶ�ѹԽ���ۼ�ʱ��,���Ϊ�����ѹ�ϸ���ʹ�ã�����ͳ��ͬʱ����
* @param[in]�� cVolt ��ǰ��ѹ
* @param[out]��qolimTime �ϸ�Խ���ۻ�ʱ������
* @return��     void
*********************************************************************
*/
int sta_volt_qualifytime_calc(uint16 qolimTimeDay[5][3], uint16 qolimTimeMon[5][3])
{
    int ret = ERR_OK;;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;
    STA_VOLT_ACCTIME_CFG_T  voltAccCfg;
    static uint16 olimTime[STA_CALC_MAX][12] = {{0}}; /*���ڴ洢��һ�ε�ѹԽ��ʱ��*/
    static uint16 qrateTime[STA_CALC_MAX][3] = {{0}}; /*���ڴ洢��һ�ε�ѹ�ϸ�ʱ��*/   
    uint16  acctime[STA_CALC_MAX][12] = {{0}};//���࣬ÿ����Խ���ޣ�Խ�����ޣ�Խ���ޣ�Խ������
    uint16  extTime[STA_CALC_MAX][3] = {{0}};       //��ѹ�ϸ�ʱ��,����   
    int alterTime[STA_CALC_MAX] = {0};    //2��Ԫ�أ�0-�գ�1-��
    
    //��ȡ��ѹԽ�޲���
    ret = sta_volt_param_read(&voltAccCfg.voltParam);
    if(ret != 0)
    {
        return ret;
    }

    /* ��ȡ�Ѿ���¼��Խ���ۻ�ʱ�䣨���ϴμ���ģ�*/ //TODO ��Ҫ�浽�ļ�����ֹ����
    memcpy(acctime, olimTime, sizeof(acctime));
    memcpy(extTime, qrateTime, sizeof(extTime));

    /* ʱ�����㣬��ѹ���ʱ�� 2��Ԫ�أ�0-�գ�1-��*/
    sta_volt_monitor_time(alterTime);

    //��ȡ����ʵʱ��ѹ
    voltAccCfg.phaseVolt.nNum = 3;
    voltAccCfg.phaseVolt.nValue[0] = g_acData.PhaAVolt;
    voltAccCfg.phaseVolt.nValue[1] = g_acData.PhaBVolt;
    voltAccCfg.phaseVolt.nValue[2] = g_acData.PhaCVolt;

    //���ɲ������߼��豸��Ϊ1
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x40100200;

    //���ɽ��߷�ʽ
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint16), &voltAccCfg.powerType, &len);
    if((ret != 0) || (voltAccCfg.powerType == PWRTYPE_UNKNOWN))
    {
        STAT_FMT_DEBUG("power type read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), powerType(%d), use default(PWRTYPE_3PHASE4).\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint16), voltAccCfg.powerType);
        voltAccCfg.powerType = PWRTYPE_3PHASE4;
        ret = ERR_OK;
    }

    voltAccCfg.interval = STA_CALC_DAY;
    voltAccCfg.addTime = alterTime[STA_CALC_DAY];

    //����Խ����ϸ��ۻ�ʱ��
    sta_volt_quaolimtime_acc(voltAccCfg, acctime[STA_CALC_DAY], extTime[STA_CALC_DAY]);

    voltAccCfg.interval = STA_CALC_MON;
    voltAccCfg.addTime = alterTime[STA_CALC_MON];

    //������Խ����ϸ����ۻ�ʱ��
    sta_volt_quaolimtime_acc(voltAccCfg, acctime[STA_CALC_MON], extTime[STA_CALC_MON]);

    /*дԽ���ۻ�ʱ��*/
    memcpy(olimTime, acctime, sizeof(olimTime));

    //���������ݴ���
    sta_volt_fault_tolerant(acctime[STA_CALC_DAY], extTime[STA_CALC_DAY]);

    /*д�ϸ��ۻ�ʱ��*/
    memcpy(qrateTime, extTime,sizeof(qrateTime));

    memcpy(qolimTimeDay[0],qrateTime[STA_CALC_DAY], sizeof(qolimTimeDay[0])); //�ϸ�
    memcpy(qolimTimeDay[1],olimTime[STA_CALC_DAY],   sizeof(qolimTimeDay[1]));//Խ����   
    memcpy(qolimTimeDay[2],olimTime[STA_CALC_DAY]+3, sizeof(qolimTimeDay[2]));//Խ���� 
    memcpy(qolimTimeDay[3],olimTime[STA_CALC_DAY]+6, sizeof(qolimTimeDay[3]));//Խ������   
    memcpy(qolimTimeDay[4],olimTime[STA_CALC_DAY]+9, sizeof(qolimTimeDay[4]));//Խ������
    
    memcpy(qolimTimeMon[0],qrateTime[STA_CALC_MON], sizeof(qolimTimeMon[0])); //�ϸ�
    memcpy(qolimTimeMon[1],olimTime[STA_CALC_MON],   sizeof(qolimTimeMon[1]));//Խ����   
    memcpy(qolimTimeMon[2],olimTime[STA_CALC_MON]+3, sizeof(qolimTimeMon[2]));//Խ���� 
    memcpy(qolimTimeMon[3],olimTime[STA_CALC_MON]+6, sizeof(qolimTimeMon[3]));//Խ������   
    memcpy(qolimTimeMon[4],olimTime[STA_CALC_MON]+9, sizeof(qolimTimeMon[4]));//Խ������  
    
    //���ջ�����嵱������,ǰ��������Ѿ�д����ӦOAD�ģ������ô���������ȡ
    if(pastdayflag)
    {
        STAT_FMT_DEBUG("past day clear qualify time.\n");
        memset(olimTime[STA_CALC_DAY], 0x00, sizeof(olimTime[STA_CALC_DAY]));
        memset(qrateTime[STA_CALC_DAY], 0x00, sizeof(qrateTime[STA_CALC_DAY]));
    }
    
    if(pastmonflag)
    {
        STAT_FMT_DEBUG("past month clear qualify time.\n");
        memset(olimTime[STA_CALC_MON], 0x00, sizeof(olimTime[STA_CALC_MON]));
        memset(qrateTime[STA_CALC_MON], 0x00, sizeof(qrateTime[STA_CALC_MON]));
    }   

    return ret;
}

/**
*********************************************************************
* @brief��      ����ѹ�ϸ������ջ�����
* @param[in]�� cVolt ��ǰ��ѹ
* @param[in]�� qRate �ϸ���
* @return��     void
*********************************************************************
*/
void sta_volt_qualifyobjs_check(OOP_OAD_U qRate, STA_CALC_INTERVAL_E *interval)
{
    *interval = STA_CALC_MON;

    if(qRate.nIndex == 1)
    {
        *interval  = STA_CALC_DAY;
    }

    return;
}

/**
*********************************************************************
* @brief��      ��ѹ�ϸ��ʼ���,��ѹ��������Χ�ڵ�ʱ��ռ�����ʱ��ı���
* @param[in]�� cVolt ��ǰ��ѹ
* @param[in]�� qRate �ϸ���
* @return��     void
*********************************************************************
*/
void sta_volt_qualifyrate_unit_calc(uint16 *extTime, OOP_VOLTQR_T *voltObj)
{   
    uint32 tmTime=0;
        
    #if UPUP_EN == 1
    tmTime = (extTime[0] + extTime[1] + extTime[2]);
    #else
    tmTime = (extTime[0] + extTime[1] + extTime[2]+extTime[3]+extTime[4]);
    #endif

    //�ն˹���ʱ��Ϊ0�Ͳ�������
    if(tmTime != 0)
    {
        //��ѹ�ϸ��ʣ�%(*100) ����-2(*100)����������,
        voltObj->dwTime1 = tmTime;           //��ѹ���ʱ��
        #if UPUP_EN == 1
        voltObj->dwTime3 = extTime[1];       //��ѹ������ʱ��
        voltObj->dwTime4 = extTime[2];       //��ѹ������ʱ��
        #else 
        voltObj->dwTime3 = extTime[1]+extTime[3];
        voltObj->dwTime4 = extTime[2]+extTime[4];
        #endif

        voltObj->wRate1  = (uint16)(extTime[0]*10000/tmTime + 0.5);   //��ѹ�ϸ���
        //voltObj->wRate2 =  (uint16)((voltObj->dwTime3 + voltObj->dwTime4)*10000/tmTime);; //��ѹԽ����   
        voltObj->wRate2 = 10000-voltObj->wRate1;
    }

    return;
}

/**
*********************************************************************
* @brief��      ��ѹ�ϸ��ʼ���,��ѹ��������Χ�ڵ�ʱ��ռ�����ʱ��ı���
* @param[in]�� cVolt ��ǰ��ѹ
* @param[in]�� qRate �ϸ���
* @return��     void
*********************************************************************
*/
void sta_volt_qualifyrate_calc(void)
{   
    int16 i;
    int ret = 0;
    uint16 extTime[5] = {0,0,0,0,0};    //��ѹ�ϸ�ʱ��
    uint16 phaseTimeDay[5][3] = {{0}};  //������պϸ�ʱ��
    uint16 phaseTimeMon[5][3] = {{0}};  //������ºϸ�ʱ��    
    OOP_VOLTSTAT_T  voltObj;//��ѹ�ϸ��ʶ���
    NOMAL_OAD_T nomalOAD = {0};
    
    ret = sta_volt_qualifytime_calc(phaseTimeDay, phaseTimeMon);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_volt_qualifytime_calc failed.\n");
        return;
    }
    
    nomalOAD.oad.value = 0x21310200; //A���ѹ�ϸ�������

    for(i = 0; i < 3; i++)
    {
        extTime[0] = phaseTimeDay[0][i]; //�ϸ�
        extTime[1] = phaseTimeDay[1][i]; //Խ����
        extTime[2] = phaseTimeDay[2][i]; //Խ����
        extTime[3] = phaseTimeDay[3][i]; //Խ������
        extTime[4] = phaseTimeDay[4][i]; //Խ������

        STAT_FMT_DEBUG("volt[%d] over limit time day: qualify(%d), up(%d), dw(%d), upup(%d), dwdw(%d)\n", 
            i, extTime[0], extTime[1], extTime[2], extTime[3], extTime[4]);

        sta_volt_qualifyrate_unit_calc(extTime, &voltObj.day);

        extTime[0] = phaseTimeMon[0][i];
        extTime[1] = phaseTimeMon[1][i];
        extTime[2] = phaseTimeMon[2][i];
        extTime[3] = phaseTimeMon[3][i];
        extTime[4] = phaseTimeMon[4][i];

        STAT_FMT_DEBUG("volt[%d] over limit time mon: qualify(%d), up(%d), dw(%d), upup(%d), dwdw(%d)\n", 
            i, extTime[0], extTime[1], extTime[2], extTime[3], extTime[4]);

        sta_volt_qualifyrate_unit_calc(extTime, &voltObj.mon);

        nomalOAD.logicId = 0;
        nomalOAD.oad.nObjID = 0x2131 + i; //�ֱ���� A,B,C��ĵ�ѹ�ϸ���
        nomalOAD.classtag = CLASS_DATA_NORMAL;
        
        ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&voltObj, sizeof(voltObj));
        if(ret != ERR_OK )
        {
            STAT_FMT_DEBUG("volt_qualifyrate_calc write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(voltObj));
            return;
        }       
    }

    return;
}

#endif

#if DESC("��ƽ���ʼ���",1)

/**
*********************************************************************
* @brief��      ��ƽ���ʼ���
* @param[in]�� data      ����
* @param[in]�� isSigned �Ƿ��з�����
* @param[out]��rate      ��ƽ����
* @return��     void
*********************************************************************
*/
int sta_imbalance_rate_calc(void *data, BOOLEAN isSigned, uint16 *rate)
{
    int ret = ERR_OK;
    uint32 max = 0;
    uint32 min = 0;
    uint32 tmp[3] = {0};
    uint8   powerType=0; 
    uint32 len = 0;    
    NOMAL_OAD_T nomalOAD = {0};    

    //���ɲ������߼��豸��Ϊ1
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x40100200;

    //û����Ĭ��Ϊ3��4��
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint16), &powerType, &len);
    if((ret != 0) || (powerType == PWRTYPE_UNKNOWN))
    {
        STAT_FMT_DEBUG("power type read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), powerType(%d), use default(PWRTYPE_3PHASE4).\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint16), powerType);
        powerType = PWRTYPE_3PHASE4;
        ret = ERR_OK;
    }

    if(isSigned)
    {
        if(powerType  == PWRTYPE_3PHASE4)//��������
        {
            tmp[0] = sta_signed_abs_calc(((int32*)data)[0]);
            tmp[1] = sta_signed_abs_calc(((int32*)data)[1]);
            tmp[2] = sta_signed_abs_calc(((int32*)data)[2]);    
        }   
        else if(powerType  == PWRTYPE_3PHASE3) //��������
        {
            //��������B��Ĭ�ϳ�A�ౣ֤���ֵ��Сֵ�ļ���
            tmp[0] = sta_signed_abs_calc(((int32*)data)[0]);
            tmp[1] = sta_signed_abs_calc(((int32*)data)[0]);
            tmp[2] = sta_signed_abs_calc(((int32*)data)[2]);    
        }
    }
    else
    {
        if(powerType  == PWRTYPE_3PHASE4) //��������
        {
            tmp[0] = ((uint16*)data)[0];
            tmp[1] = ((uint16*)data)[1];
            tmp[2] = ((uint16*)data)[2];
        }
        else if(powerType  == PWRTYPE_3PHASE3) //��������
        {
            //��������B��Ĭ�ϳ�A�ౣ֤���ֵ��Сֵ�ļ���
            tmp[0] = ((uint16*)data)[0];
            tmp[1] = ((uint16*)data)[0];    
            tmp[2] = ((uint16*)data)[2];             
        }        
    }
        
    max =  sta_dlmax_calc(3,tmp);
    min =  sta_dlmin_calc(3,tmp);
   
    if(max != 0)
    {
        *rate = (uint16)((float32)(max-min)*100/max+0.5);
    }

    return ret;
}

/**
*********************************************************************
* @brief��      ������ƽ���ʼ���
* @param[in]�� stTSA     ���ܱ��ַ
* @param[out]��pstMeter ���ܱ���
* @return��     void
*********************************************************************
*/
void sta_curr_imbalance_calc(void)
{
    int ret = ERR_OK;
    uint16  ultemp_cur = 0; //������ƽ���
    int32   curr[3];          //�������    
    uint32 len = 0; 
    OOP_INT3V_T rtData = {0};    
    NOMAL_OAD_T nomalOAD = {0};    
     
    //��ȡʵʱ����
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x20010200;

    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_INT3V_T), (uint8*)&rtData, &len);
    if((ret != 0) || (rtData.nNum > 4))
    {
        STAT_FMT_DEBUG("real-time current read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), nNum(%d).\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_INT3V_T), rtData.nNum);
    
        return;
    }
    
    memcpy(curr, rtData.nValue, sizeof(curr));

    //��ƽ���ʼ���
    ret = sta_imbalance_rate_calc(curr, 1, &ultemp_cur); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_imbalance_rate_calc failed.\n");
        return;
    }

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x20270200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    
    //������ƽ����
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&ultemp_cur, sizeof(uint16)); 
    if(ret < 0)
    {
        STAT_FMT_DEBUG("current imbalance rate write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint16));
    
        return;
    }

    return;
}

/**
*********************************************************************
* @brief��      ��ѹ��ƽ���ʼ���
* @param[in]�� stTSA     ���ܱ��ַ
* @param[out]��pstMeter ���ܱ���
* @return��     void
*********************************************************************
*/
void sta_volt_imbalance_calc(void)
{
    int ret = ERR_OK;
    uint16  ultemp_vlt = 0; //��ѹ��ƽ���
    uint16  volt[3];        //�����ѹ      
    uint32 len = 0;    
    OOP_WORD3V_T rtData = {0};       
    NOMAL_OAD_T nomalOAD = {0};    
    
    //��ȡʵʱ��ѹ
    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x20000200;
    
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_WORD3V_T), (uint8*)&rtData, &len);
    if((ret != 0) || (rtData.nNum > 3))
    {
        STAT_FMT_DEBUG("real-time voltage read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d), nNum(%d).\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_WORD3V_T), rtData.nNum);
    
        return;
    }

    //����һ������
    arrcpy(volt, sizeof(uint16), rtData.nValue, sizeof(uint32), 3);
      
    //��ƽ���ʼ���
    ret = sta_imbalance_rate_calc(volt, 0, &ultemp_vlt); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_imbalance_rate_calc failed.\n");
        return;
    }

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = 0x20260200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    
    //��ѹ��ƽ����
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&ultemp_vlt, sizeof(uint16));  
    if(ret < 0)
    {
        STAT_FMT_DEBUG("current imbalance rate write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint16));
    
        return;
    } 
    
    return;
}

#endif

#if DESC("���ӵ�������",1)

int sta_basic_energy_deal(const STA_MTR_INFO_T * mtrInfo, uint8 rtValid, const int64 *rtData, int64 *lastData)
{
    int i = 0;
    
    if(rtValid == FALSE)  // 
    {
        STAT_FMT_DEBUG("****���������\n");
        
        if(mtrInfo->type == MTR_PULSE)
        {
            if(rtData[0] < 5000)                    //ʵʱ��С��0.5�ȵĻ�,��0��ʼ  
            {
               for(i = 0;i < 5;i++)
               {
                  lastData[i] = 0;                  //����������ʼ��Ϊ0
               }
            }
            else                                    //ʵʱ�����ڵ���0.5�ȵĻ�,��¼����   
            {
               for(i = 0;i < 5;i++)
               {
                   lastData[i] = rtData[i];
               }
            }  
        }
        else                                        //500 485��0.05�ȼ���
        {
            for(i = 0;i < 5;i++)
            {
               lastData[i] = rtData[i];
            }
        }
    }
    else  //��������
    {
        for(i = 0;i < 5;i++)
        {
            if(lastData[i] == NONELLWORD)
            {
                STAT_FMT_DEBUG("****����[%d]��Ч���\n", i);
                
                if(mtrInfo->type == MTR_PULSE)
                {
                    if(rtData[0] < 5000)
                    {
                        lastData[i] = 0;
                    }
                    else
                    {
                        lastData[i] = rtData[i];
                    }
                }
                else
                {
                    lastData[i] = rtData[i];
                }
            }
            
        }
    }    

    return 0;
}

/**
*********************************************************************
* @brief��      д1���ӵ�������
* @param[in]�� cEnergy  ����OAD
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void 
*********************************************************************
*/
int sta_basic_1minen_write(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffsetcs, void *curData)
{
    int     ret = 0;
    char    pfile[20] = {0};  //TODO ˽���ļ�·�� ���ݲ�����ŵõ���
    char    pfilebak[25] = {0};  //�����ļ�
    uint8  lastnew[41] = {0};

    //˽���ļ���485����Ҫ64����������8��������1����
    switch(mtrInfo->type)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
        default: return -1;
    }
    
    //�����ļ���
    sprintf(pfilebak, "bak%s", pfile);

    memcpy(&lastnew, curData, 40);
    lastnew[40] = sta_checksum_calc(lastnew, 40);

    /*���µ�������*/
    ret = write_pdata_xram(pfile,(char *)lastnew,dataOffsetcs+STA_MINCS_ENERGY_BASE_SEG,STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("���µ�������ʧ��ret=%d\n", ret);
    }
    
    /*���µ�����������*/
    ret = write_pdata_xram(pfilebak,(char *)lastnew,dataOffsetcs+STA_MINCS_ENERGY_BASE_SEG,STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("���µ������ݵ���ʧ��ret=%d\n", ret);
    }

    return ret;
}

/**
*********************************************************************
* @brief��      ��������1���ӵ���
* @param[in]�� cEnergy  ����OAD
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void 
*********************************************************************
*/
int sta_basic_1minen_read(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffsetcs, void *basic)
{
    int     ret;
    char    pfile[20] = {0};  //TODO ˽���ļ�·�� ���ݲ�����ŵõ���
    char    pfilebak[25] = {0};  //�����ļ�
    uint16  seg_addr = dataOffsetcs + STA_MINCS_ENERGY_BASE_SEG;
    uint8  mtrType = mtrInfo->type;
    uint8  lastnew[41] = {0};
    uint8  cs = 0;

    //˽���ļ���485����Ҫ64����������8��������1����
    switch(mtrType)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
        default: return -1;
    }

    //�����ļ���
    sprintf(pfilebak, "bak%s", pfile);
    
    /*��ȡ˽�������ܶ���(�������)*/
    ret = read_pdata_xram(pfile,(char *)lastnew, seg_addr, STA_MINCS_ENERGY_UNIT_SIZE);
    FREEZE_BUF_TRACE(lastnew, 41, "%s��ȡ����:",pfile);
    
    //����У���
    cs = sta_checksum_calc(lastnew, 40);
    //��ʧ�ܻ�У��ʧ����������ļ�
    if((ret != 0) || (cs != lastnew[40]))
    {
        memset(lastnew, 0, sizeof(lastnew));
        ret = read_pdata_xram(pfilebak,(char *)lastnew, seg_addr, STA_MINCS_ENERGY_UNIT_SIZE);
        
        STAT_BUF_DEBUG(lastnew, 41, "%s��ȡ��������:",pfilebak);
        cs = sta_checksum_calc(lastnew, 40);
        if((ret != 0) || (cs != lastnew[40])) //������ʧ�ܻ�У��ʧ����������ļ�
        {
            return ERR_INVALID;
        }
    }

    //��ȡ��ȷ�Ĵ���
    memcpy(basic, &lastnew, STA_MIN_ENERGY_UNIT_SIZE);

    return ERR_OK;
}

/**
*********************************************************************
* @brief��      д1���ӵ�������
* @param[in]�� cEnergy  ����OAD
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void 
*********************************************************************
*/
int sta_basic_1minen_clear(const STA_MTR_INFO_T * mtrInfo)
{
    int     ret = 0;
    char    pfile[20];  //TODO ˽���ļ�·�� ���ݲ�����ŵõ���
    char datapath[256] = {0};
    char bakdatapath[256] = {0};

    //ɾ�����в�����
    if((mtrInfo->type == MTR_485)&&(mtrInfo->nIndex == 0xffff))
    {
        sprintf(datapath,"/data/app/%s/data/statmeter*",ADV_APP_NAME);

        ret = wildcard_remove(datapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
        }

        sprintf(bakdatapath,"/data/app/%s/data/bakstatmeter*",ADV_APP_NAME);

        ret = wildcard_remove(bakdatapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", bakdatapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", bakdatapath);
        }
    }
    else
    {
        //˽���ļ���485����Ҫ64����������8��������1����
        switch(mtrInfo->type)
        {
            case MTR_AC:    sprintf(pfile,"statacsample");break;
            case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
            case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
            default: return -1;
        }
        
        sprintf(datapath,"/data/app/%s/data/%s",ADV_APP_NAME, pfile);

        ret = remove(datapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
        }

        sprintf(bakdatapath,"/data/app/%s/data/bak%s",ADV_APP_NAME, pfile);

        ret = remove(bakdatapath);
        if(ret != 0)
        {
            PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", bakdatapath, ret, strerror(errno));
        }
        else
        {
            PRTL_FMT_DEBUG("remove \"%s\" success.\n", bakdatapath);
        }
    }

    //����ͬ��������
    sync();
    
    return ret;
}

/**
*********************************************************************
* @brief��      д������1���ӵ���
* @param[in]�� cEnergy  ����OAD
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void 
*********************************************************************
*/
int sta_delta_1minen_write(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffsetcs, void *delta)
{
    int     ret = 0;
    char    pfile[20] = {0};  //TODO ˽���ļ�·�� ���ݲ�����ŵõ���
    uint8   deltacs[41] = {0};  //��CSУ��Ĳ�ֵ����

    //˽���ļ���485����Ҫ64����������8��������1����
    switch(mtrInfo->type)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
        default: return -1;
    }

    memcpy(deltacs, delta, STA_MIN_ENERGY_UNIT_SIZE);
    
    deltacs[40] = sta_checksum_calc(delta, 40);
    
    /*���·��ӵ���*/
    ret = write_pdata_xram(pfile,(char *)deltacs,dataOffsetcs+STA_MINCS_ENERGY_DATA_SEG,STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("���·��ӵ���ʧ��ret=%d\n", ret);
    }
    
    return ret;
}

/**
*********************************************************************
* @brief��      ��������1���ӵ���
* @param[in]�� cEnergy  ����OAD
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void 
*********************************************************************
*/
int sta_delta_1minen_read(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffsetcs, void *delta)
{
    int     ret = 0;
    char    pfile[20] = {0};  //TODO ˽���ļ�·�� ���ݲ�����ŵõ���
    uint8   deltacs[41] = {0};  //��CSУ��Ĳ�ֵ����
    uint16  segOffset = dataOffsetcs + STA_MINCS_ENERGY_DATA_SEG;
    uint8  cs = 0;

    //˽���ļ���485����Ҫ64����������8��������1����
    switch(mtrInfo->type)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xff);break;
        default: return -1;
    }

    /*��ȡ˽�������ܶ���*/
    ret = read_pdata_xram(pfile,(char *)deltacs, segOffset, STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_TRACE("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    cs = sta_checksum_calc(deltacs, 40);
    if(deltacs[40] != cs)
    {
        STAT_FMT_DEBUG("У��ʧ��\n");
        return ERR_CHK;
    }

    memcpy(delta, deltacs, STA_MIN_ENERGY_UNIT_SIZE);
    
    return ret;
}

/**
*********************************************************************
* @brief��      ��ȡ�ϴε�������,���ϴε�ʵʱ����
* @param[in]�� cEnergy  ����OAD
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void 
*********************************************************************
*/
int sta_lastbasic_data_read(const STA_MTR_INFO_T * mtrInfo, uint32 dataOffset, const int64 *curData, int64 *lstData)
{
    int     ret = 0;
    
    ret = sta_basic_1minen_read(mtrInfo, dataOffset, (uint8*)lstData);
    if(ret != ERR_OK)
    {
        sta_basic_energy_deal(mtrInfo, FALSE, curData, lstData);
    }
    else
    {
        sta_basic_energy_deal(mtrInfo, TRUE, curData, lstData);
    }

    return ERR_OK;
}

/**
*********************************************************************
* @brief��      ��ȡʵʱ����
* @param[in]�� cEnergy  ����OAD
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void 
*********************************************************************
*/
int sta_realtime_data_read(const STA_MTR_INFO_T * mtrInfo,  OOP_OAD_U srcOAD, int64 *curData)
{
    NOMAL_OAD_T nomalOAD = {0};       
    READ_RECORD_T readRecord;
    RECORD_UNIT_T recordUnit;
    OOP_ENERGY_T energy;
    int ret = 0;
    uint32 len = 0;
    uint32 i = 0;
    
    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&recordUnit, sizeof(recordUnit));
    MEMZERO(&energy, sizeof(energy));

    if(mtrInfo->type == MTR_AC)
    {
        nomalOAD.logicId = 1;
        nomalOAD.oad.value = srcOAD.value;
        
        ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
            return ret;
        }
    }
    else if(mtrInfo->type == MTR_PULSE)
    {
        nomalOAD.oad.value = srcOAD.value;
        ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
        if(ret != ERR_OK)
        {
            STAT_FMT_TRACE("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_ENERGY_T));
            return ret;
        }
    }
    else if(mtrInfo->type == MTR_485) //�ɼ��ĵ����ǰ���OAD=0�ļ�¼�ʹ洢��
    {
        readRecord.recordOAD.road.oadCols.nNum = 1;
        readRecord.recordOAD.road.oadCols.oad[0].value = srcOAD.value; 
        readRecord.recordOAD.infoNum = mtrInfo->nIndex; //����TSA�������������
        readRecord.cType = COL_TM_END;
        readRecord.cEnd = time(NULL) + COMPENSATION_TIME+1; // ע��+8��Сʱ(28800)ת�ɸ���ʱ�䣬�����ǿ�����
        readRecord.cStart = (readRecord.cEnd/60)*60-60;  //ɸѡ��ǰ����1�����ڵ�����

        //��Ҫ������ģ�����MACУ���㷨
        readRecord.MAC.nNum = 6;
        memcpy(readRecord.MAC.value, mtrInfo->tsa.add, 6);  
        
        ret = db_read_record(g_staUdpHandle, &readRecord, sizeof(energy), &recordUnit);
        if(ret != ERR_OK)
        {
            STAT_BUF_TRACE(readRecord.MAC.value, 6, "collect energy read failed. ret(%d) logicId(%d), infoNum(%d), option(%d), oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, readRecord.recordOAD.logicId, readRecord.recordOAD.infoNum, readRecord.recordOAD.optional, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value, sizeof(energy));
            return ret;
        }

        memcpy(&energy, recordUnit.record[0].outDataBuf, sizeof(energy));
    }
    else
    {
        ;//NOTHING TO DO
    }

    if(energy.nNum > OOP_FULL_RATES)
    {
        STAT_FMT_DEBUG("energy.nNum(%d) > OOP_FULL_RATES(%d).\n", energy.nNum, OOP_FULL_RATES);
        return ret;
    }
    
    arrcpy(curData, sizeof(int64), energy.nValue, sizeof(uint32), energy.nNum);

    //����жϵ�����Ч�Ժ�ȫFF
    if(mtrInfo->type == MTR_485)
    {
        for(i = 0; i < 5; i++)
        {
            if(0 == get_bit_value((uint8*)&energy.rsv,32,i) 
                || energy.nValue[i] == 0xFFFFFFFF)
            {
                curData[i] = 0;
            }
        }
    }
    
    //���ɺ�485��������-2�� �����������-4��ͳһת��Ϊ�ܼ����������-4
    if(mtrInfo->type != MTR_PULSE)
    {
        for(i = 0; i < 5; i++)
        {
            curData[i] = curData[i]*100;
        }
    }

    return ret;      
}

/**
*********************************************************************
* @brief��      �������������
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void 
*********************************************************************
*/
void sta_mpoint_energy_calc(const STA_MTR_INFO_T * mtrInfo, OOP_OAD_U srcOAD, uint32 dataOffsetcs)
{
    int     i;
    int64  curData[5] = {0,0,0,0,0};     /*��ǰ��������8�ֽڣ�*/
    int64  lstData[5] = {0,0,0,0,0};   /*ǰһ���ӵ�����*/
    int64  detaData[5] = {0,0,0,0,0};       /*���ӵ���*/
    int ret = 0;
    char *typechr[4] = {"����", "����", "����", "����"};    
    char *typepchr[4] = {"����", "����","����", "����"};
    char *mtrichr[7] = {"485", "ģ���", "����", "����", "����", "�ز�", "�ز��๦��"};
    uint8 backFlag = FALSE; //�������߱��

    //ÿ�μ���ǰ��������ӵ���
    ret = sta_delta_1minen_write(mtrInfo, dataOffsetcs, detaData); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("������ӵ���ʧ��ret=%d\n", ret);
    }

    //��ȡ��ǰ����ʾֵ
    ret = sta_realtime_data_read(mtrInfo, srcOAD, curData);
    if(ret != 0) 
    {
        STAT_FMT_TRACE("sta_rt_data_read failed.\n");
        return;
    }

    //��ȡ�ϴε���ʾֵ(�������)
    ret = sta_lastbasic_data_read(mtrInfo, dataOffsetcs, curData, lstData);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_lst_basic_data_read failed.\n");
        return;
    }
    
    //������ӵ���
    for(i = 0; i < 5; i++)
    {
        if(curData[i] < lstData[i]) /*��ǰ�����ȵ���С�����*/
        {
            backFlag = TRUE;
            break;
        }
    }

    if(backFlag)
    {    
        STAT_FMT_DEBUG("��������\n");
        memset(&detaData, 0, sizeof(detaData));
    }
    else
    {
        //������ӵ���
        for(i = 0; i < 5; i++)
        {
            detaData[i] = curData[i] - lstData[i];
        }
    }

    if(detaData[0] != (detaData[1] + detaData[2] + detaData[3] + detaData[4]))
    {
        STAT_FMT_DEBUG("�ܵ����쳣 �� %lld ���ʵ����� %lld\n", 
            detaData[0], detaData[1] + detaData[2] + detaData[3] + detaData[4]);
    }

    if(mtrInfo->type != MTR_PULSE)
    {
        STAT_FMT_DEBUG("�ϴ�%sʾֵ Pn %d %s �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
            typechr[(srcOAD.nObjID>>4)-1], mtrInfo->nIndex, mtrichr[mtrInfo->type-1], lstData[0],lstData[1],lstData[2],lstData[3],lstData[4]);
        STAT_FMT_DEBUG("ʵʱ%sʾֵ Pn %d %s �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
            typechr[(srcOAD.nObjID>>4)-1], mtrInfo->nIndex, mtrichr[mtrInfo->type-1], curData[0],curData[1],curData[2],curData[3],curData[4]);
        STAT_FMT_DEBUG("����%s���� Pn %d %s �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
            typechr[(srcOAD.nObjID>>4)-1], mtrInfo->nIndex, mtrichr[mtrInfo->type-1], detaData[0],detaData[1],detaData[2],detaData[3],detaData[4]);
    }
    else
    {
        STAT_FMT_DEBUG("�ϴ�%sʾֵ Pn %d %s �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
            typepchr[srcOAD.attID-15], mtrInfo->pulseOAD.nObjID&0xff, mtrichr[mtrInfo->type-1], lstData[0],lstData[1],lstData[2],lstData[3],lstData[4]);
        STAT_FMT_DEBUG("ʵʱ%sʾֵ Pn %d %s �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
            typepchr[srcOAD.attID-15], mtrInfo->pulseOAD.nObjID&0xff, mtrichr[mtrInfo->type-1], curData[0],curData[1],curData[2],curData[3],curData[4]);
        STAT_FMT_DEBUG("����%s���� Pn %d %s �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
            typepchr[srcOAD.attID-15], mtrInfo->pulseOAD.nObjID&0xff, mtrichr[mtrInfo->type-1], detaData[0],detaData[1],detaData[2],detaData[3],detaData[4]);
    }
    
    //������ӵ�������
    ret = sta_basic_1minen_write(mtrInfo, dataOffsetcs, curData);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("������ӵ�������ʧ��ret=%d\n", ret);
    }

    /*������ӵ���*/ 
    ret = sta_delta_1minen_write(mtrInfo, dataOffsetcs, detaData); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("������ӵ���ʧ��ret=%d\n", ret);
    }
}


/**
*********************************************************************
* @brief��      �����������ڣ����㽻�ɣ����壬485��ķ��ӵ���
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void 
*********************************************************************
*/
void sta_mpoint_entry(void)
{
    OOP_OAD_U oad = {0};
    STA_MTR_INFO_T mtrInfo;
    OOP_METER_T meter;
    uint32 i = 0;
    uint8 cnt = 0;
    
    MEMZERO(&mtrInfo, sizeof(mtrInfo));
    MEMZERO(&meter, sizeof(meter));

    //���㽻�ɷ��ӵ���
    mtrInfo.type = MTR_AC;
    
    //���������й�
    oad.value = 0x00100200;
    sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_ACTCS_ENERGY_OFFSET);

    //���㷴���й�
    oad.value = 0x00200200;
    sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_ACTCS_ENERGY_OFFSET);

    //���������޹�
    oad.value = 0x00300200;
    sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_RCTCS_ENERGY_OFFSET);

    //���㷴���޹�
    oad.value = 0x00400200;
    sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_RCTCS_ENERGY_OFFSET);

    //�ȴ���������׼��OK
    while(!pulse_get_ok() && (cnt++ < 20))
    {
        usleep(100000);
    }
    usleep(100000);

    //����������ӵ���
    mtrInfo.type = MTR_PULSE;

    oad.value = 0x24010200;
    for(i = 0; i < 8; i++)
    {
        oad.nObjID = 0x2401 + i;
        
        mtrInfo.pulseOAD.value = oad.value;

        //���������й�
        oad.attID = 15;
        sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_ACTCS_ENERGY_OFFSET);

        //���㷴���й�
        oad.attID = 17;
        sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_ACTCS_ENERGY_OFFSET);

        //���������޹�
        oad.attID = 16;
        sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_RCTCS_ENERGY_OFFSET);

        //���㷴���޹�
        oad.attID = 18;
        sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_RCTCS_ENERGY_OFFSET);
    }

    //����485����ӵ���
    for(i = 0; i < oopMeterList.oopMeterNum; i++)
    {
        if(app_get_meter_list_by_id(i, &meter) == 0)
        {
            if(meter.basic.port.nObjID == 0xf201)
            {
                mtrInfo.type = MTR_485;
            }
            else
            {
                continue;
            }
            
            mtrInfo.nIndex = meter.nIndex;
            memcpy(&mtrInfo.tsa, &meter.basic.tsa, sizeof(OOP_TSA_T));

            //���������й�
            oad.value = 0x00100200;
            sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_ACTCS_ENERGY_OFFSET);

            //���㷴���й�
            oad.value = 0x00200200;
            sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_ACTCS_ENERGY_OFFSET);

            //���������޹�
            oad.value = 0x00300200;
            sta_mpoint_energy_calc(&mtrInfo, oad, STA_POS_RCTCS_ENERGY_OFFSET);   

            //���㷴���޹�
            oad.value = 0x00400200;
            sta_mpoint_energy_calc(&mtrInfo, oad, STA_NEG_RCTCS_ENERGY_OFFSET);
        }
    }

    return;
}

#endif

#if DESC("������ؼ���",1)
/**
*********************************************************************
* @brief��      ���������й�������Positive active electric energy���㣨����ͬʱͳ�ƣ�
* @param[in]�� tgODA �ܼ������
* @param[in]�� minCalc �ܼ������
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void
*********************************************************************
*/
void sta_pulse_pae_calc(OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    STA_MTR_INFO_T mtrInfo;
    uint32 i = 0;
    int64 minEnergy[5] = {0}; /* ���⡢�塢ƽ���ȣ����ӵ�������(�й�) */
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;
    uint32 len = 0;

    nomalOAD.oad.value = pulseOAD.value;

    mtrInfo.type = MTR_PULSE;
    mtrInfo.pulseOAD.value = pulseOAD.value;

    //�����й�
    MEMZERO(minEnergy, sizeof(minEnergy));
    sta_delta_1minen_read(&mtrInfo, STA_POS_ACTCS_ENERGY_OFFSET, minEnergy);
    
    nomalOAD.oad.attID = 7;//���������й�����
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //����ʣ��ܼ��ƽ��
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("����%d�����е��� �� %d ����1 %d ����2 %d ����3 %d ����4 %d\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    nomalOAD.oad.attID = 8;//���������й�����
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //����ʣ��ܼ��ƽ��
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("month positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("����%d�����е��� �� %d ����1 %d ����2 %d ����3 %d ����4 %d\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    return;
}

/**
*********************************************************************
* @brief��      ���������޹�������Positive reactive electric energy���㣨����ͬʱͳ�ƣ�
* @param[in]�� tgODA �ܼ������
* @param[in]�� minCalc �ܼ������
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void
*********************************************************************
*/
void sta_pulse_pre_calc(OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    STA_MTR_INFO_T mtrInfo;
    uint32 i = 0;
    int64 minEnergy[5] = {0}; /* ���⡢�塢ƽ���ȣ����ӵ�������(�й�) */
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;
    uint32 len = 0;

    nomalOAD.oad.value = pulseOAD.value;

    mtrInfo.type = MTR_PULSE;
    mtrInfo.pulseOAD.value = pulseOAD.value;

    //�����޹�   
    MEMZERO(minEnergy, sizeof(minEnergy));
    sta_delta_1minen_read(&mtrInfo, STA_POS_RCTCS_ENERGY_OFFSET, minEnergy);
    
    nomalOAD.oad.attID = 11;//���������޹�����
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //����ʣ��ܼ��ƽ��
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day positive reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("����%d�����޵��� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    nomalOAD.oad.attID = 12;//���������޹�����
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //����ʣ��ܼ��ƽ��
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("month positive reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("����%d�����޵��� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    return;
}

/**
*********************************************************************
* @brief��      ���巴���й�������negative active electric energy���㣨����ͬʱͳ�ƣ�
* @param[in]�� tgODA �ܼ������
* @param[in]�� minCalc �ܼ������
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void
*********************************************************************
*/
void sta_pulse_nae_calc(OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    STA_MTR_INFO_T mtrInfo;
    uint32 i = 0;
    int64 minEnergy[5] = {0}; /* ���⡢�塢ƽ���ȣ����ӵ�������(�й�) */
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;
    uint32 len = 0;

    nomalOAD.oad.value = pulseOAD.value;

    mtrInfo.type = MTR_PULSE;
    mtrInfo.pulseOAD.value = pulseOAD.value;

    //�����й�
    MEMZERO(minEnergy, sizeof(minEnergy));
    sta_delta_1minen_read(&mtrInfo, STA_NEG_ACTCS_ENERGY_OFFSET, minEnergy);
    
    nomalOAD.oad.attID = 9;//���շ����й�����
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //����ʣ��ܼ��ƽ��
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("����%d�շ��е��� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    nomalOAD.oad.attID = 10;//���·����й�����
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //����ʣ��ܼ��ƽ��
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("month negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("����%d�·��е��� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    return;
}

/**
*********************************************************************
* @brief��      ���巴���޹�������negative reactive electric energy���㣨����ͬʱͳ�ƣ�
* @param[in]�� tgODA �ܼ������
* @param[in]�� minCalc �ܼ������
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void
*********************************************************************
*/
void sta_pulse_nre_calc(OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    STA_MTR_INFO_T mtrInfo;
    uint32 i = 0;
    int64 minEnergy[5] = {0}; /* ���⡢�塢ƽ���ȣ����ӵ�������(�й�) */
    NOMAL_OAD_T nomalOAD = {0};       
    OOP_ENERGY_T energy;
    uint32 len = 0;

    nomalOAD.oad.value = pulseOAD.value;

    mtrInfo.type = MTR_PULSE;
    mtrInfo.pulseOAD.value = pulseOAD.value;

    //�����޹�
    MEMZERO(minEnergy, sizeof(minEnergy));
    sta_delta_1minen_read(&mtrInfo, STA_NEG_RCTCS_ENERGY_OFFSET, minEnergy);
    
    nomalOAD.oad.attID = 13;//���շ����޹�����
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //����ʣ��ܼ��ƽ��
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day negative reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    if(pastdayflag)
    {
        STAT_FMT_DEBUG("����%d�շ��޵��� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    nomalOAD.oad.attID = 14;//���·����޹�����
    
    MEMZERO(&energy, sizeof(energy));
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_ENERGY_T), (uint8*)&energy, &len);
    
    energy.nNum = 5; //����ʣ��ܼ��ƽ��
    
    for(i = 0; i < energy.nNum; i++)
    {
        energy.nValue[i] += minEnergy[i];
    }
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&energy, sizeof(OOP_ENERGY_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("month negative reactive power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    }        

    if(pastmonflag)
    {
        STAT_FMT_DEBUG("����%d�·��޵��� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n",
            pulseOAD.nObjID&0xff, energy.nValue[0], energy.nValue[1], energy.nValue[2], energy.nValue[3], energy.nValue[4]);

    }

    return;
}


/**
*********************************************************************
* @brief��      ������������㣨����ͬʱͳ�ƣ�
* @param[in]�� tgODA �ܼ������
* @param[in]�� minCalc �ܼ������
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void
*********************************************************************
*/
void sta_pulse_energy_calc(void)
{
    OOP_OAD_U oad = {0};
    uint32 i = 0;
    
    oad.value = 0x24010200;
    for(i = 0; i < 8; i++)
    {
        oad.nObjID = 0x2401 + i;

        sta_pulse_pae_calc(oad);
        sta_pulse_pre_calc(oad);
        sta_pulse_nae_calc(oad);
        sta_pulse_nre_calc(oad);
    }

    return;
}

#endif


#if DESC("�ܼ��鹫��",1)

/**
*********************************************************************
* @brief��      ��Ա�ܼӼ���
* @param[in]�� ptct      ������Ϣ
* @param[in]�� opFlag   ������
* @param[in]�� tgMember �ܼӳ�Ա��������
* @param[out]��tgResult �ܼӽ����������
* @param[in]�� nNum      �ܼӳ�Ա������
* @Create    : ����
* @Date      ��2020-04-17
* @return��     void
*********************************************************************
*/
void sta_virtual_tgcalc(const OOP_PTCT_T *ptct, const uint8 opFlag, const int64 *tgMember, int64 *tgResult, uint8 nNum)
{
    uint8 i = 0;

    if(opFlag == 1)
    {
        for(i = 0; i < nNum; i++)
        {
            tgResult[i] -= (tgMember[i]*(ptct->ct)*(ptct->pt));
        }
    }
    else if(opFlag == 0)
    {
        for(i = 0; i < nNum; i++)
        {
            tgResult[i] += (tgMember[i]*(ptct->ct)*(ptct->pt));
        }        
    }
}

/**
*********************************************************************
* @brief��      ��ȡ�ܼ������Ա�
* @param[in]��  mtrid   �������
* @param[out]��pstPara �ܼ������
* @return��     void
*********************************************************************
*/
int sta_virtual_param_read(OOP_OAD_U tgOAD, OOP_TGSETS_T *pstPara)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 len = 0;
    OOP_OAD_U inOAD = {0x23010200}; //�ܼ������Ա�

    inOAD.nObjID = tgOAD.nObjID;
    nomalOAD.logicId = 0;
    memcpy(&nomalOAD.oad, &inOAD, sizeof(OOP_OAD_U));

    /* ��ȡ�ܼ������Ա� */
    ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_TGSETS_T), (uint8*)pstPara, &len);
    if(ret != 0)
    {
        STAT_FMT_TRACE("tg param read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_TGSETS_T));
    
        return ret;
    }
    
    return ret;
}

/**
*********************************************************************
* @brief��      ��ȡ������ϵ��
* @param[in]��  pstMtrInfo  ��������Ϣ
* @param[out]��pstPTCT      ������ϵ��
* @return��     void
*********************************************************************
*/
int sta_virtual_ptct_read(const STA_MTR_INFO_T *pstMtrInfo, OOP_PTCT_T *pstPTCT)
{
    uint32 ct = 0;
    uint32 pt = 0;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_METER_T meter;   //�����
    int ret = 0;
    uint32 len = 0;

    //���ɵĻ�����ϵ��0x401c0200-������������� 0x401d0200-��ѹ���������
    if(pstMtrInfo->type == MTR_AC)
    {
        nomalOAD.logicId = 0; //TODO
        nomalOAD.oad.value = 0x401c0200;

        ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint32), (uint8*)&ct, &len);

        nomalOAD.oad.value = 0x401d0200;
        
        ret |=  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(uint32), (uint8*)&pt, &len);

        if(ret != 0)
        {
            return ret;
        }
        else
        {
            pstPTCT->ct = ct;
            pstPTCT->pt = pt;
        }
    }

    //���������Ļ���������
    if(pstMtrInfo->type == MTR_PULSE)
    {
        nomalOAD.logicId = 0;
        nomalOAD.oad.value = pstMtrInfo->pulseOAD.value;
        nomalOAD.oad.attID = 3;
        
        ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_PTCT_T), (uint8*)pstPTCT, &len);
        if(ret != 0)
        {
            return ret;
        }     
    }

    //485������Ļ���������
    if(pstMtrInfo->type == MTR_485)
    {
        ret =  app_get_meter_list(pstMtrInfo->nIndex, &meter);
        if(ret != 0)
        {
            return ret;
        }
        else
        {
            pstPTCT->ct = meter.extend.CT;
            pstPTCT->pt = meter.extend.PT;
        }
    }

    return ret;    
}

/**
*********************************************************************
* @brief��      ���������Ķ�ȡ�ܼӵ�����
* @param[in]�� tgOAD �ܼ���OAD
* @param[out]�� actEnergy �й���������
* @param[out]�� rctEnergy �޹���������
* @return��     void
*********************************************************************
*/
int sta_virtual_energy_read(OOP_OAD_U tgOAD, int64 actEnergy[2][5], int64 rctEnergy[2][5])
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HENERGYS_T henergys; 
    uint32 len = 0;

    MEMZERO(&henergys, sizeof(henergys));   

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 7; 
    
    ret |= db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(henergys), (uint8*)&henergys, &len);

    ret |= memcpy_s(actEnergy[0], sizeof(actEnergy[0]), henergys.nValue, henergys.nNum*sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return ret;
    }

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 8; 

    ret |= db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(henergys), (uint8*)&henergys, &len);

    ret |= memcpy_s(rctEnergy[0], sizeof(rctEnergy[0]), henergys.nValue, henergys.nNum*sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return ret;
    }


    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 9; 

    ret |= db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(henergys), (uint8*)&henergys, &len);

    ret |= memcpy_s(actEnergy[1], sizeof(actEnergy[1]), henergys.nValue, henergys.nNum*sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return ret;
    }

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 10; 

    ret |= db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(henergys), (uint8*)&henergys, &len);

    ret |= memcpy_s(rctEnergy[1], sizeof(rctEnergy[1]), henergys.nValue, henergys.nNum*sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return ret;
    }

    return ret;
}

/**
*********************************************************************
* @brief��      �洢�ܼӵ�����
* @param[in]�� cEnergy ��ǰ����(��ʹ�ã��ܼ��谴���ö�ȡ�������͵������޷�ͳһ����)
* @param[in]�� tgODA �ܼ������
* @return��     void
*********************************************************************
*/
void sta_virtual_energy_write(OOP_OAD_U tgOAD, int64 actEnergy[2][5], int64 rctEnergy[2][5])
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HENERGYS_T henergys;

    MEMZERO(&henergys, sizeof(henergys));
    
    //�洢�ܼ�������0x23010700�����й�����0x23010800�����޹�����0x23010900�����й�����0x23010a00�����޹���
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 7; 

    henergys.nNum = 5; //����ʣ��ܼ��ƽ��
    ret = memcpy_s(henergys.nValue, sizeof(henergys.nValue), actEnergy[0], sizeof(actEnergy[0]));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return;
    }
            
    db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(henergys));

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 8; 

    henergys.nNum = 5; //����ʣ��ܼ��ƽ��
    ret = memcpy_s(henergys.nValue, sizeof(henergys.nValue), rctEnergy[0], sizeof(rctEnergy[0]));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return;
    }

    db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(henergys));

    //TODO ���£������ݴ������£��嵱������

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 9; 

    henergys.nNum = 5; //����ʣ��ܼ��ƽ��
    ret = memcpy_s(henergys.nValue, sizeof(henergys.nValue), actEnergy[1], sizeof(actEnergy[1]));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return;
    }

    db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(henergys));

    nomalOAD.oad.nObjID = tgOAD.nObjID;
    nomalOAD.oad.attID = 10; 

    henergys.nNum = 5; //����ʣ��ܼ��ƽ��
    ret = memcpy_s(henergys.nValue, sizeof(henergys.nValue), rctEnergy[1], sizeof(rctEnergy[1]));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("memcpy_s failed. ret(%d)\n", ret);
        return;
    }

    db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)&henergys, sizeof(henergys));
}

/**
*********************************************************************
* @brief��      ��ȡ�ܼ����Ա���ӵ���
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @Create    : ����
* @Date      ��2020-04-15
* @return��     void
*********************************************************************
*/
void sta_virtual_member_min_energy_get(const STA_MTR_INFO_T *mtrInfo, const OOP_TG_T *tgParm, int64 *actEnergy, int64 *reactEnergy)
{
    if( tgParm->tgFlag == 1 ) //1-����
    {
        //���㷴���й����޹�
        sta_delta_1minen_read(mtrInfo, STA_NEG_ACTCS_ENERGY_OFFSET, actEnergy);

        sta_delta_1minen_read(mtrInfo, STA_NEG_RCTCS_ENERGY_OFFSET, reactEnergy);
    }
    else  /* ȡ���򣨹��ʡ����������� 0-����*/ 
    {
        //���������й����޹�
        sta_delta_1minen_read(mtrInfo, STA_POS_ACTCS_ENERGY_OFFSET, actEnergy);

        sta_delta_1minen_read(mtrInfo, STA_POS_RCTCS_ENERGY_OFFSET, reactEnergy);            
    }
}

pthread_mutex_t g_fTG_flock[OOP_MAX_TG];

/**
*********************************************************************
* @brief��      �洢�ܼӷ��ӵ������������
* @param[in]�� cEnergy ��ǰ����(��ʹ�ã��ܼ��谴���ö�ȡ�������͵������޷�ͳһ����)
* @param[in]�� tgODA �ܼ������
* @return��     void
*********************************************************************
*/
void sta_tg_min_elec_init()
{
    uint8 i  = 0;

    for(i = 0; i < OOP_MAX_TG; i++)
    {
        pthread_mutex_init(&(g_fTG_flock[i]), NULL);
    }
}


/**
*********************************************************************
* @brief��      �洢�ܼӷ��ӵ������������
* @param[in]�� cEnergy ��ǰ����(��ʹ�ã��ܼ��谴���ö�ȡ�������͵������޷�ͳһ����)
* @param[in]�� tgODA �ܼ������
* @return��     void
*********************************************************************
*/
void sta_tg_min_elec_write(OOP_OAD_U tgOAD, int64 actEnergy[5])
{
    uint16  segOffset = 0;
    char    pfile[20] = {0};  //˽���ļ�·�� ���ݲ�����ŵõ���
    int ret = ERR_OK;
    uint8   tgactcs[41] = {0};  //��CSУ��Ĳ�ֵ����

    sprintf(pfile,"stattgsets%d",tgOAD.nObjID&0xff);

    memcpy(tgactcs, actEnergy, STA_MIN_ENERGY_UNIT_SIZE);

    tgactcs[40] = sta_checksum_calc((uint8*)actEnergy, 40);

    //�й������洢
    segOffset = STA_TGMINCS_ENERGY_DATA_SEG + STA_ACTCS_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile, (char *)tgactcs, segOffset, STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_tg_min_acte_write failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;
    }

    g_tgStoreNo++;
}

/**
*********************************************************************
* @brief��       ��ȡ�ܼӷ��ӵ�������������
* @param[in]��   tgODA     �ܼ���OAD
* @param[out]�� acteTotal �ܼ����й��ܵ���
* @param[out]�� storeNo   �洢���
* @Create    :  ����
* @Date      �� 2020-04-20
* @return��      void
*********************************************************************
*/
int sta_tg_min_elec_read(OI tgOI, int64 *acteTotal, uint8 num)
{
    uint16  segOffset = 0;
    char    pfile[20] = {0};  //˽���ļ�·�� ���ݲ�����ŵõ���
    int ret = ERR_OK;
    uint8   tgactcs[41] = {0};  //��CSУ��Ĳ�ֵ����
    uint8  cs = 0;

    sprintf(pfile,"stattgsets%d",tgOI&0xff);

    //�й�������ȡ
    segOffset = STA_TGMINCS_ENERGY_DATA_SEG + STA_ACTCS_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile, (char *)tgactcs, segOffset, STA_MINCS_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_tg_min_acte_read failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    cs = sta_checksum_calc(tgactcs, 40);
    if(tgactcs[40] != cs)
    {
        return ERR_CHK;
    }
    
    memcpy(acteTotal, tgactcs, num * sizeof(int64));

    return 0;
}


/**
*********************************************************************
* @brief��      �洢�ܼӷ��ӵ������������
* @param[in]�� cEnergy ��ǰ����(��ʹ�ã��ܼ��谴���ö�ȡ�������͵������޷�ͳһ����)
* @param[in]�� tgODA �ܼ������
* @return��     void
*********************************************************************
*/
void sta_tg_min_acte_write(OOP_OAD_U tgOAD, int64 actEnergy[5])
{
    uint8 index = 0;
    int64 totalEnergy[5] = {0};
    int   ret = 0;

    index = (tgOAD.nObjID & 0xff);
    if(0 == index || index > OOP_MAX_TG)
    {
        return;
    }

    PTHREAD_LOCK_SAFE(&(g_fTG_flock[index - 1]));
    ret = sta_tg_min_elec_read(tgOAD.nObjID, totalEnergy, 5);
    if(0 != ret)
    {
        memset(totalEnergy, 0, sizeof(totalEnergy));
    }
    totalEnergy[0] += actEnergy[0];
    totalEnergy[1] += actEnergy[1];
    totalEnergy[2] += actEnergy[2];
    totalEnergy[3] += actEnergy[3];
    totalEnergy[4] += actEnergy[4];

    sta_tg_min_elec_write(tgOAD, totalEnergy);

    PTHREAD_UNLOCK_SAFE(&(g_fTG_flock[index - 1]));
}

/**
*********************************************************************
* @brief��       ��ȡ�ܼӷ��ӵ�������������
* @param[in]��   tgODA     �ܼ���OAD
* @param[out]�� acteTotal �ܼ����й��ܵ���
* @param[out]�� storeNo   �洢���
* @Create    :  ����
* @Date      �� 2020-04-20
* @return��      void
*********************************************************************
*/
int sta_tg_min_acte_read(OI tgOI, int64 *acteTotal, uint32 *storeNo)
{
    uint8 index = 0;
    int64 totalEnergy[5] = {0};
    int   ret = 0;
    OOP_OAD_U tgOAD;

    index = (tgOI & 0xff);
    if(0 == index || index > OOP_MAX_TG)
    {
        return -1;
    }

    PTHREAD_LOCK_SAFE(&(g_fTG_flock[index - 1]));
    ret = sta_tg_min_elec_read(tgOI, totalEnergy, 5);
    if(0 == ret)
    {
        *acteTotal = totalEnergy[0];
    }
    else
    {
        *acteTotal = 0;
    }

    *storeNo = g_tgStoreNo;
    /* ���, ��ֹ��λ�����֮ǰ�ĵ��� */
    if(0 != *acteTotal)
    {
        memset(totalEnergy, 0, sizeof(totalEnergy));
        tgOAD.value = 0x23010200;
        tgOAD.nObjID = tgOI;
        sta_tg_min_elec_write(tgOAD, totalEnergy);
    }
    PTHREAD_UNLOCK_SAFE(&(g_fTG_flock[index - 1]));

    return ret;
}

#endif

#if DESC("�ܼ����������",0)
void sta_virtual_energy_period_calc(const STA_MTR_INFO_T * mtrInfo,  uint32 dataOffset, OOP_OAD_U srcOAD, int64 *minEnergy)
{
    int     i;
    int64  rtData[5] = {0,0,0,0,0};     /*��ǰ��������8�ֽڣ�*/
    int64  lastData[5] = {0,0,0,0,0};   /*ǰһ���ӵ�����*/
    int64  rtEn[5] = {0,0,0,0,0};       /*���ӵ���*/
    char    pfile[20];  //˽���ļ�·�� ���ݲ�����ŵõ���

    //����Ҫ������ӵ�����ֱ��ȡ�ϴμ���Ľ��
    if(g_MinuteCalcEn == FALSE)
    {
        sta_lst_min_data_read(mtrInfo, dataOffset, minEnergy);    
        STAT_FMT_TRACE("minute energy no need to calculate.\n");    
        return;
    }

    /*-��ȡ��ǰʱ�̲ɼ�������-*/
    if(sta_rt_data_read(mtrInfo, srcOAD, rtData) < 0) 
    {
        STAT_FMT_DEBUG("sta_virtual_rtdata_read failed.\n");
        return;
    }

    /*��ȡ�ϴε��ܶ���(�������)*/
    if(sta_lst_basic_data_read(mtrInfo, dataOffset, rtData, lastData)<0)
    {
        STAT_FMT_DEBUG("sta_virtual_lstdata_read failed.\n");
        return;
    }
    
    //������ӵ���
    for(i = 0;i < 5;i++)
    {
        //���м�û�г�ͨ����ʱ��rtData[i] == NONELLWORD
        if(rtData[i] == NONELLWORD)
        {
            rtData[i] = lastData[i];
        }
        
        if(rtData[i] < lastData[i]) /*��ǰ�����ȵ���С�����*/
            lastData[i] = rtData[i];

        if(rtData[i] >= lastData[i])
        {
            rtEn[i] = rtData[i] - lastData[i];  /*���㵱ǰ���ӵ���*/
        }
        else                    
        {
            rtEn[i] = 9999999900LL - lastData[i] + 1 + rtData[i];   //����������
        }
    }

#if TOTAL_EN_CHECK == 1
    //У��һ�£���ֹ�ܵ��������ڷַ��ʵ���
    if(rtEn[0] != (rtEn[1] + rtEn[2] + rtEn[3] + rtEn[4]))
    {
        char tmpBuf[300];
        memset(tmpBuf, 0x0, sizeof(tmpBuf));
        sprintf(tmpBuf, "CalEnMin�������ȣ���У�� rtEn = %lld %lld %lld %lld %lld", 
            rtEn[0], rtEn[1], rtEn[2], rtEn[3], rtEn[4]);
        writelog("/mnt/log/stat.log",tmpBuf);

        rtEn[0] = rtEn[1] + rtEn[2] + rtEn[3] + rtEn[4];
    }
#endif

    memcpy((char *)minEnergy, (char *)rtEn, sizeof(lastData));
    memcpy((char *)lastData, (char *)rtData, sizeof(lastData));

    //˽���ļ���485����Ҫ64����������8��������1����
    switch(mtrInfo->type)
    {
        case MTR_AC:    sprintf(pfile,"statacsample");break;
        case MTR_485:   sprintf(pfile,"statmeter%d",mtrInfo->nIndex);break;
        case MTR_PULSE: sprintf(pfile,"statpluse%d",mtrInfo->pulseOAD.nObjID&0xf);break;
        default: return;
    }

    /*���µ�������*/
    write_pdata_xram(pfile,(char *)lastData,dataOffset+STA_MIN_ENERGY_BASE_SEG,STA_MIN_ENERGY_UNIT_SIZE);
    /*������ӵ���*/   
    write_pdata_xram(pfile,(char *)rtEn,dataOffset+STA_MIN_ENERGY_DATA_SEG,STA_MIN_ENERGY_UNIT_SIZE); 
}

/**
*********************************************************************
* @brief��      �ܼ����Ա�ĵ������㣬����ÿ��������һ�Σ�������Ϊ���ӵ���
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void
*********************************************************************
*/
void sta_virtual_energy_member_calc(const STA_MTR_INFO_T *mtrInfo, const OOP_TG_T *tgParm, int64 *actEnergy, int64 *reactEnergy)
{
    OOP_OAD_U oad;

    //���ɺ�485��������ӵ�������
    if((mtrInfo->type == MTR_AC)||(mtrInfo->type == MTR_485))
    {
        if( tgParm->tgFlag == 1 ) //1-����
        {
            //���㷴���й����޹�
            oad.value = 0x00200200;
            sta_virtual_energy_period_calc(mtrInfo, STA_NEG_ACT_ENERGY_OFFSET, oad, actEnergy);

            oad.value = 0x00400200;
            sta_virtual_energy_period_calc(mtrInfo, STA_NEG_RCT_ENERGY_OFFSET, oad, reactEnergy);
        }
        else  /* ȡ���򣨹��ʡ����������� 0-����*/ 
        {
            //���������й����޹�
            oad.value = 0x00100200;
            sta_virtual_energy_period_calc(mtrInfo, STA_POS_ACT_ENERGY_OFFSET, oad, actEnergy);

            oad.value = 0x00300200;
            sta_virtual_energy_period_calc(mtrInfo, STA_POS_RCT_ENERGY_OFFSET, oad, reactEnergy);
        }
    }

    //�����������ӵ�������
    if(mtrInfo->type == MTR_PULSE)
    {
        oad.value = mtrInfo->pulseOAD.value;
        if( tgParm->tgFlag == 1 ) //1-����
        {
            //���㷴���й����޹�
            oad.attID = 17; //�����й�������ֵ
            sta_virtual_energy_period_calc(mtrInfo, STA_NEG_ACT_ENERGY_OFFSET, oad, actEnergy);

            oad.attID = 18; //�����޹�������ֵ
            sta_virtual_energy_period_calc(mtrInfo, STA_NEG_RCT_ENERGY_OFFSET, oad, reactEnergy);
        }
        else  /* ȡ���򣨹��ʡ����������� 0-����*/ 
        {
            //���������й����޹�
            oad.attID = 15; //�����й�������ֵ
            sta_virtual_energy_period_calc(mtrInfo, STA_POS_ACT_ENERGY_OFFSET, oad, actEnergy);

            oad.attID = 16; //�����޹�������ֵ
            sta_virtual_energy_period_calc(mtrInfo, STA_POS_RCT_ENERGY_OFFSET, oad, reactEnergy);
        }        
    } 
}

/**
*********************************************************************
* @brief��      ��˽���ļ���ȡ�ܼӵ�����
* @param[in]�� cEnergy ��ǰ����(��ʹ�ã��ܼ��谴���ö�ȡ�������͵������޷�ͳһ����)
* @param[in]�� tgODA �ܼ������
* @return��     void
*********************************************************************
*/
int sta_virtual_energy_get(OOP_OAD_U tgOAD, int64 actEnergy[2][5], int64 rctEnergy[2][5])
{
    uint16  segOffset = 0;
    char    pfile[20];  //˽���ļ�·�� ���ݲ�����ŵõ���
    int ret = ERR_OK;

    sprintf(pfile,"stattgsets%d",tgOAD.nObjID&0xf);

    //���й�������ȡ
    segOffset = STA_DAY_ENERGY_DATA_SEG + STA_ACT_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile,(char *)actEnergy[0], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    //���޹�������ȡ
    segOffset = STA_DAY_ENERGY_DATA_SEG + STA_RCT_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile,(char *)rctEnergy[0], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    //���й�������ȡ
    segOffset = STA_MON_ENERGY_DATA_SEG + STA_ACT_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile,(char *)actEnergy[1], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }

    //���޹�������ȡ
    segOffset = STA_MON_ENERGY_DATA_SEG + STA_RCT_ENERGY_OFFSET;
    ret = read_pdata_xram(pfile,(char *)rctEnergy[1], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return ret;        
    }  

    return ret;
}

/**
*********************************************************************
* @brief��      �洢�ܼӵ�����
* @param[in]�� cEnergy ��ǰ����(��ʹ�ã��ܼ��谴���ö�ȡ�������͵������޷�ͳһ����)
* @param[in]�� tgODA �ܼ������
* @return��     void
*********************************************************************
*/
void sta_virtual_energy_backup(OOP_OAD_U tgOAD, int64 actEnergy[2][5], int64 rctEnergy[2][5])
{
    uint16  segOffset = 0;
    char    pfile[20];  //˽���ļ�·�� ���ݲ�����ŵõ���
    int ret = ERR_OK;

    sprintf(pfile,"stattgsets%d",tgOAD.nObjID&0xf);

    //���գ������������
    if(pastdayflag)
    {
        memset_s(actEnergy[0], sizeof(actEnergy[0]), 0, sizeof(actEnergy[0]));
        memset_s(rctEnergy[0], sizeof(rctEnergy[0]), 0, sizeof(rctEnergy[0]));        
    }

    //���£������������
    if(pastmonflag)
    {
        memset_s(actEnergy[1], sizeof(actEnergy[1]), 0, sizeof(actEnergy[1]));
        memset_s(rctEnergy[1], sizeof(rctEnergy[1]), 0, sizeof(rctEnergy[1]));        
    }    

    //���й������洢
    segOffset = STA_DAY_ENERGY_DATA_SEG + STA_ACT_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile,(char *)actEnergy[0], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;        
    }

    //���޹������洢
    segOffset = STA_DAY_ENERGY_DATA_SEG + STA_RCT_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile,(char *)rctEnergy[0], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;        
    }

    //���й������洢
    segOffset = STA_MON_ENERGY_DATA_SEG + STA_ACT_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile,(char *)actEnergy[1], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;        
    }

    //���޹������洢
    segOffset = STA_MON_ENERGY_DATA_SEG + STA_RCT_ENERGY_OFFSET;
    ret = write_pdata_xram(pfile,(char *)rctEnergy[1], segOffset, STA_MIN_ENERGY_UNIT_SIZE);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read_pdata_xram failed.ret(%d), file(%s), segOffset(%d)\n",
            ret, pfile, segOffset);
        return;        
    }       
}

/**
*********************************************************************
* @brief��      �ܼӵ��������㣨����ͬʱͳ�ƣ�
* @param[in]�� tgODA �ܼ������
* @param[in]�� minCalc �ܼ������
* @return��     void
*********************************************************************
*/
void sta_virtual_energy_calc(OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    int64 tgActAccEnergy[2][5] = {{0}};    /* �գ��⡢�塢ƽ���ȣ������ܼӱ���(�й�) */
    int64 tgRctAccEnergy[2][5] = {{0}};    /* �գ��⡢�塢ƽ���ȣ������ܼӱ���(�޹�) */

    int64 actEnergy[5] = {0}; /* ���⡢�塢ƽ���ȣ����ӵ�������(�й�) */
    int64 rctEnergy[5] = {0}; /* ���⡢�塢ƽ���ȣ����ӵ�������(�޹�) */

    OOP_TGSETS_T tgParam;    //�ܼ������
    uint16 i;
    STA_MTR_INFO_T mtrInfo; //��������Ϣ
    OOP_PTCT_T ptct;        //������ϵ��

    /*- �������ܼӵļ�����������Ϣ -*/
    ret = sta_virtual_param_read(tgOAD, &tgParam);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_virtual_param_read failed. ret(%d), tgOAD(0x%08x)\n", ret, tgOAD.value);
        return;
    }

    //���ϴμ�����ܼ������ݣ��ܣ��⡢�塢ƽ���ȣ�
    ret = sta_virtual_energy_get(tgOAD, tgActAccEnergy, tgRctAccEnergy);    
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_virtual_energy_read failed. ret(%d), tgOAD(0x%08x)\n", ret, tgOAD.value);
    }
    
    //���������ܼӵĵ��(����485�����ɣ�����)
    for(i = 0; i < tgParam.nNum; i++)
    {
        //��ȡ��������Ϣ
        ret = sta_mtrtype_read(&tgParam.tg[i].add, &mtrInfo);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_mtrtype_read failed. ret(%d),tgid(%d),tgOAD(0x%8x)\n",ret, i, tgOAD.value);
            STAT_BUF_DEBUG(&tgParam.tg[i].add, sizeof(OOP_TSA_T), "tsa:");
            continue;
        }

        //��ȡ����������
        ret = sta_virtual_ptct_read(&mtrInfo, &ptct);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_virtual_ptct_read failed. ret(%d), mtrType(%d)\n",ret, mtrInfo.type);
            continue;
        }
        
        //��ȡ�й��޹���������
        //sta_virtual_energy_member_calc(&mtrInfo, &tgParam.tg[i], actEnergy, rctEnergy);
        sta_virtual_member_min_energy_get(&mtrInfo, &tgParam.tg[i], actEnergy, rctEnergy);
        
        //�й������ܼ�,����Ҫ���������ݺ�������
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActAccEnergy[0], 5);
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActAccEnergy[1], 5);
        
        //�޹������ܼ�,����Ҫ���������ݺ�������        
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, rctEnergy, tgRctAccEnergy[0], 5);
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, rctEnergy, tgRctAccEnergy[1], 5);        
    }

    //�洢�ܼӽ��
    sta_virtual_energy_write(tgOAD, tgActAccEnergy, tgRctAccEnergy); 

    //�洢һ�ݵ�˽���ļ�
    sta_virtual_energy_backup(tgOAD, tgActAccEnergy, tgRctAccEnergy);
}
#endif

#if DESC("�ܼ��鹦�ʼ���",1)
/**
*********************************************************************
* @brief��      ��ȡʵʱ����
* @param[in]�� cEnergy  ����OAD
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @Create    : ����
* @Date      ��2020-04-17
* @return��     void 
*********************************************************************
*/
int sta_rt_power_read(const STA_MTR_INFO_T * mtrInfo, OOP_OAD_U srcOAD, int64 *rtPower)
{
    NOMAL_OAD_T nomalOAD = {0};       
    READ_RECORD_T readRecord;
    RECORD_UNIT_T recordUnit;
    OOP_INT4V_T  powerPhase = {0};
    int power = 0;
    int ret = 0;
    uint32 len = 0;

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&recordUnit, sizeof(recordUnit));
    
    if(mtrInfo->type == MTR_AC)
    {
        nomalOAD.logicId = 1;
        nomalOAD.oad.value = srcOAD.value;
        
        ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(OOP_INT4V_T), (uint8*)&powerPhase, &len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int));
            return ret;
        }

        power = powerPhase.nValue[0];
    }
    else if(mtrInfo->type == MTR_PULSE)
    {
        nomalOAD.oad.value = srcOAD.value;
        ret = db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(int), (uint8*)&power, &len);
        if(ret != ERR_OK)
        {
            STAT_FMT_TRACE("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int));
            return ret;
        }
    }
    else if(mtrInfo->type == MTR_485) //�ɼ��Ĺ����ǰ���OAD=0�ļ�¼�ʹ洢��
    {
        readRecord.recordOAD.road.oadCols.nNum = 1;
        readRecord.recordOAD.road.oadCols.oad[0].value = srcOAD.value; 
        readRecord.recordOAD.infoNum = mtrInfo->nIndex; //����TSA�������������
        readRecord.cType = COL_TM_END;
        readRecord.cEnd = time(NULL)+1 + COMPENSATION_TIME; // ע��+8��Сʱ(28800)ת�ɸ���ʱ��
        readRecord.cStart = readRecord.cEnd-86400;
        OOP_INT4V_T *ptPowerDate = NULL;

        //��Ҫ������ģ�����MACУ���㷨
        readRecord.MAC.nNum = 6;
        memcpy(readRecord.MAC.value, mtrInfo->tsa.add, 6);  
        
        ret = db_read_record(g_staUdpHandle, &readRecord, sizeof(int), &recordUnit);
        if(ret != ERR_OK)
        {
            STAT_BUF_TRACE(readRecord.MAC.value, 6, "collect energy read failed. ret(%d) logicId(%d), infoNum(%d), option(%d), oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
               ret, readRecord.recordOAD.logicId, readRecord.recordOAD.infoNum, readRecord.recordOAD.optional, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value, sizeof(int));
            return ret;
        }
        ptPowerDate = (OOP_INT4V_T *)recordUnit.record[0].outDataBuf;
        power = ptPowerDate->nValue[0];        
    }
    else
    {
        ;//NOTHING TO DO
    }

    *rtPower = (int64)power;
    
    return ret;      
}

/**
*********************************************************************
* @brief��      д�ܼӹ���
* @param[in]�� tgOAD �ܼ���OAD
* @param[in]�� actData �й�����
* @param[in]�� rctData �޹�����
* @return��     void
*********************************************************************
*/
void sta_tg_power_write(OOP_OAD_U tgOAD, int64 actData, int64 rctData)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    
    //�洢�ܼ�������0x23010300���й����ʣ���0x23010400���޹����ʣ�
    normal.classtag = CLASS_DATA_NORMAL;
    normal.oad.nObjID = tgOAD.nObjID;

    normal.oad.attID = 3;    
    ret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&actData, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(int64));
    }

    normal.oad.attID = 4; 
    ret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&rctData, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(int64));
    }

    return;
}

/**
*********************************************************************
* @brief��      �ܼ����Ա�Ĺ��ʶ�ȡ
* @param[in]�� tgParm  �ܼ������
* @param[out]: actEnergy �й�����
* @param[out]: reactEnergy �޹�����
* @return��     void
*********************************************************************
*/
void sta_tg_member_power_read(const STA_MTR_INFO_T *mtrInfo, const OOP_TG_T *tgParm, int64 *actData, int64 *rctData)
{
    OOP_OAD_U oad;

    //���ɺ�485��������ӵ�������
    if((mtrInfo->type == MTR_AC)||(mtrInfo->type == MTR_485))
    {
        if( tgParm->tgFlag == 1 ) //1-����
        {
            //�й�����
            oad.value = 0x20040200;
            sta_rt_power_read(mtrInfo, oad, actData);

            //������һ���Ǹ����������ֵ����ʾ������Ϊ0
            *actData = (*actData < 0)?(*actData):0;
            *actData = abs_int64(*actData);

            //�޹�����
            oad.value = 0x20050200;
            sta_rt_power_read(mtrInfo, oad, rctData);
            
            //������һ���Ǹ����������ֵ����ʾ������Ϊ0
            *rctData = (*rctData < 0)?(*rctData):0;         
            *rctData = abs_int64(*rctData);
        }
        else  /* ȡ���򣨹��ʡ����������� 0-����*/ 
        {
            //�й�����
            oad.value = 0x20040200;
            sta_rt_power_read(mtrInfo, oad, actData);

            //������һ������������Ǹ�ֵ����ʾ������Ϊ0
            *actData = (*actData > 0)?(*actData):0;

            //�޹�����
            oad.value = 0x20050200;
            sta_rt_power_read(mtrInfo, oad, rctData);
            
            //������һ������������Ǹ�ֵ����ʾ������Ϊ0
            *rctData = (*rctData > 0)?(*rctData):0;
        }
    }

    //�����������ӵ�������
    if(mtrInfo->type == MTR_PULSE)
    {
        oad.value = mtrInfo->pulseOAD.value;
        if( tgParm->tgFlag == 1 ) //1-����
        {
            //�й�����
            oad.attID = 5;
            sta_rt_power_read(mtrInfo, oad, actData);

            //������һ���Ǹ����������ֵ����ʾ������Ϊ0
            *actData = (*actData < 0)?(*actData):0;
            *actData = abs_int64(*actData);

            //�޹�����
            oad.attID = 6; 
            sta_rt_power_read(mtrInfo, oad, rctData);
            
            //������һ���Ǹ����������ֵ����ʾ������Ϊ0
            *rctData = (*rctData < 0)?(*rctData):0;       
            *rctData = abs_int64(*rctData);
        }
        else  /* ȡ���򣨹��ʡ����������� 0-����*/ 
        {
            //�й�����
            oad.attID = 5;
            sta_rt_power_read(mtrInfo, oad, actData);

            //������һ������������Ǹ�ֵ����ʾ������Ϊ0
            *actData = (*actData > 0)?(*actData):0;

            //�޹�����
            oad.attID = 6; 
            sta_rt_power_read(mtrInfo, oad, rctData);
            
            //������һ������������Ǹ�ֵ����ʾ������Ϊ0
            *rctData = (*rctData > 0)?(*rctData):0;
        }        
    } 
}

#endif

#if DESC("�ܼ�����ؼ���",1)

/**
*********************************************************************
* @brief��      �ܼ��黬��ʱ����ƽ�����ʼ���,�����ʵĻ���ƽ��ֵ,�������ڼ��������ڴ�С
* @param[in]�� tgODA �ܼ������
* @param[in]�� tgActPower �ܼ����й�����
* @param[in]�� tgRctPower �ܼ����޹�����
* @Create    : ����
* @Date      ��2020-04-17
* @return��     void
*********************************************************************
*/
void sta_tg_slip_power_calc(OOP_OAD_U tgOAD, int64 tgActPower, int64 tgRctPower)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0}; 
    uint8 slipPeriod = 0;           //��������
    uint32 len = 0;
    int64 slipActPowerAve[8] = {0}; //����ƽ���й�����
    int64 slipRctPowerAve[8] = {0}; //����ƽ���޹�����
    uint32 tgID = tgOAD.nObjID - 0x2301; //�ܼ���ID
    uint32 slipID = g_slippower_fifo[tgID].nCount; //������������    
    int slipCount = 0;     //���ƻ���������ݵ���
    int i = 0;

    //��ȡ��������
    normal.oad.value = tgOAD.value;
    normal.oad.attID = 13;

    ret = db_read_nomal(g_staUdpHandle, &normal, sizeof(uint8), (uint8*)&slipPeriod, &len);
    if((ret != ERR_OK) || (slipPeriod == 0))
    {
        STAT_FMT_TRACE("slip period read failed, slip period: %d, default slip period = 1 min. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            slipPeriod, ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(uint8));

        slipPeriod = 1;
    }

    //�����ǻ����������ֵ�������Ǹ��ݻ����������ã���Ϊ�˻������ڱ���С������¶������ݿ���
    if(g_slippower_fifo[tgID].nCount <= STA_SLIPPERIOD_MAX)
    {
        g_slippower_fifo[tgID].nCount++;
    }
    else //����������ȣ����һ�����ݳ��ӣ��µ���ӵ�β��
    {
        memmove(&g_slippower_fifo[tgID].slipAct[0], &g_slippower_fifo[tgID].slipAct[1], STA_SLIPPERIOD_MAX*sizeof(int64));
        memmove(&g_slippower_fifo[tgID].slipRct[0], &g_slippower_fifo[tgID].slipRct[1], STA_SLIPPERIOD_MAX*sizeof(int64));
        slipID = STA_SLIPPERIOD_MAX;
    }

    //�������ݴ洢��������У����ݳ�ʼ��ʱ�������Ҫ���
    g_slippower_fifo[tgID].slipAct[slipID] = tgActPower;
    g_slippower_fifo[tgID].slipRct[slipID] = tgRctPower;

    //���㻬�������ڵ��ۼ�ֵ�����ݸ���δ���������ڣ������и������㣬���򰴻������ڼ���
    //���ڲ���MAX���㶼���޷�����������MAX������޷��������Ƚ�ʱ���з��ŵ�i�ᱻ�������޷�����������Ҫǿת��
    slipCount = MAX(0, (int)(slipID-slipPeriod+1));
    if(slipID-slipCount+1 > slipPeriod)
    {
        STAT_FMT_DEBUG("�ܼ���%d��������쳣 �������� %d ������� %d\n", tgOAD.nObjID&0xff, slipPeriod, slipID-slipCount+1);
    }
    else
    {
        for(i = slipID; i >= slipCount; i--)
        {
            slipActPowerAve[tgID] += g_slippower_fifo[tgID].slipAct[i];
            slipRctPowerAve[tgID] += g_slippower_fifo[tgID].slipRct[i];
        }
    }

    slipActPowerAve[tgID] /= MIN(slipID+1, slipPeriod);
    slipRctPowerAve[tgID] /= MIN(slipID+1, slipPeriod);

    STAT_FMT_DEBUG("�ܼ���%d����� �й� %lld �޹� %lld �������� %d ������� %d\n", tgOAD.nObjID&0xff, slipActPowerAve[tgID], slipRctPowerAve[tgID], slipPeriod, slipID-slipCount+1);
    
    normal.classtag = CLASS_DATA_NORMAL;

    normal.oad.attID = 5; //�����й�   
    ret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&slipActPowerAve[tgID], sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(int64));
    }

    normal.oad.attID = 6; //�����޹�
    ret = db_write_nomal(g_staUdpHandle, &normal, (uint8*)&slipRctPowerAve[tgID], sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, normal.logicId, normal.infoNum, normal.oad.value, sizeof(int64));
    }

    return;
}

/**
*********************************************************************
* @brief��      �ܼ�����������
* @param[in]�� tgODA �ܼ������
* @param[in]�� minCalc �ܼ������
* @Create    : ����
* @Date      ��2020-04-17
* @return��     void
*********************************************************************
*/
void sta_tg_data_clear(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = 0;
    NOMAL_OAD_T nomalOAD = {0};
    RECORD_OAD_T recordOAD = {0};
    OOP_HENERGYS_T henergys;
    int oad_tmp[6] = {3,4,7,8,9,10};//�������Ҫɾ���ܼ�������attID
    nomalOAD.oad.value = tgOAD.value;
    uint32 tgID = tgOAD.nObjID - 0x2301; //�ܼ���ID
    int64 zeropower = 0;   //����

    MEMZERO(&henergys, sizeof(henergys));
    henergys.nNum = 5;

    nomalOAD.oad.attID = 7;//���й�����
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
    
        return;
    } 
    
    nomalOAD.oad.attID = 8;//���޹�����
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
    
        return;
    }

    nomalOAD.oad.attID = 9;//���й�����
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day positive active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
    
        return;
    } 
    
    nomalOAD.oad.attID = 10;//���޹�����
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&henergys, sizeof(OOP_HENERGYS_T));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
    
        return;
    } 

    recordOAD.classtag = CLASS_DATA_NORMAL;
    recordOAD.optional = 0;
    recordOAD.road.oadCols.nNum = 1;
    recordOAD.road.oadCols.oad[0].value = tgOAD.value;
    //ɾ����������
    recordOAD.road.oadMain.value = 0x50020200;
    //��6��ɾ��
    for(int tmp_index = 0; tmp_index < 6; tmp_index++ )
    {
        recordOAD.road.oadCols.oad[0].attID = oad_tmp[tmp_index];  
        ret = sta_delete_record(fd, &recordOAD);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }

    //ɾ���ն�������
    recordOAD.road.oadMain.value = 0x50040200;
    for(int tmp_index = 0; tmp_index < 6; tmp_index++ )
    {
        recordOAD.road.oadCols.oad[0].attID = oad_tmp[tmp_index];  
        ret = sta_delete_record(fd, &recordOAD);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }

    //ɾ���¶�������
    recordOAD.road.oadMain.value = 0x50060200;
    for(int tmp_index = 0; tmp_index < 6; tmp_index++ )
    {
        recordOAD.road.oadCols.oad[0].attID = oad_tmp[tmp_index];  
        ret = sta_delete_record(fd, &recordOAD);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("day negative active power write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
                ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(OOP_HENERGYS_T));
        
            return;
        } 
    }

    //��ջ���ʶ���
    MEMZERO(&g_slippower_fifo[tgID], sizeof(STA_SLIPPOWER_FIFO_T));

    //���㹦��
    nomalOAD.classtag = CLASS_DATA_NORMAL;

    nomalOAD.oad.attID = 3; //�й�   
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&zeropower, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int64));
    }

    nomalOAD.oad.attID = 4; //�޹�
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&zeropower, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int64));
    }

    nomalOAD.oad.attID = 5; //�����й�   
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&zeropower, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int64));
    }

    nomalOAD.oad.attID = 6; //�����޹�
    ret = db_write_nomal(fd, &nomalOAD, (uint8*)&zeropower, sizeof(int64));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("tg power write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n", 
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(int64));
    }

    STAT_FMT_DEBUG("�ܼ���%d��ɾ������\n", tgOAD.nObjID&0xff);

    return;
}

/**
*********************************************************************
* @brief��      �ܼ�����㣨����ͬʱ���㣩
* @param[in]�� tgODA �ܼ������
* @param[in]�� minCalc �ܼ������
* @Create    : ����
* @Date      ��2020-04-17
* @return��     void
*********************************************************************
*/
void sta_tg_data_calc(OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    int64 tgActAccPower = 0;   // �й������ܼ�
    int64 tgRctAccPower = 0;   // �޹������ܼ�
    int64 actPower = 0;   // �ܼӳ�Ա�й�����
    int64 rctPower = 0;   // �ܼӳ�Ա�޹�����  
    
    int64 tgActAccEnergy[STA_CALC_MAX][5] = {{0}};    /* �գ��⡢�塢ƽ���ȣ������ܼӱ���(�й�) */
    int64 tgRctAccEnergy[STA_CALC_MAX][5] = {{0}};    /* �գ��⡢�塢ƽ���ȣ������ܼӱ���(�޹�) */

    int64 actEnergy[5] = {0}; /* ���⡢�塢ƽ���ȣ����ӵ�������(�й�) */
    int64 rctEnergy[5] = {0}; /* ���⡢�塢ƽ���ȣ����ӵ�������(�޹�) */
    int64 tgActEnergy[5] = {0}; /* ���⡢�塢ƽ���ȣ��ܼӷ��ӵ�������(�й�) */
    OOP_TGSETS_T tgParam;    //�ܼ������
    uint16 i;
    STA_MTR_INFO_T mtrInfo = {0}; //��������Ϣ
    OOP_PTCT_T ptct;        //������ϵ��
    char *mtrichr[7] = {"485", "ģ���", "����", "����", "����", "�ز�", "�ز��๦��"};

    /*- �������ܼӵļ�����������Ϣ -*/
    ret = sta_virtual_param_read(tgOAD, &tgParam);
    if((ret != 0) || (tgParam.nNum == 0))
    {
        return;
    }

    //����Խ����
    if(tgParam.nNum > OOP_MAX_TGSETS)
    {
        STAT_FMT_DEBUG("tgOAD(0x%08x), tgNum(%d) > max(%d)\n", tgOAD.value, tgParam.nNum, OOP_MAX_TGSETS);
        return;
    }

    STAT_FMT_DEBUG("�ܼ���%d���㣺��Ա�� %d.\n", tgOAD.nObjID&0xff, tgParam.nNum);

    //���ϴμ�����ܼӵ������ݣ��ܣ��⡢�塢ƽ���ȣ�
    ret = sta_virtual_energy_read(tgOAD, tgActAccEnergy, tgRctAccEnergy);    
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_virtual_energy_read failed. ret(%d), tgOAD(0x%08x)\n", ret, tgOAD.value);
    }
    
    //���������ܼӵĵ��(����485�����ɣ�����)
    for(i = 0; i < tgParam.nNum; i++)
    {
        //��ȡ��������Ϣ
        ret = sta_mtrtype_read(&tgParam.tg[i].add, &mtrInfo);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_mtrtype_read failed. ret(%d),tgid(%d),tgOAD(0x%8x)\n",ret, i, tgOAD.value);
            STAT_BUF_DEBUG(&tgParam.tg[i].add, sizeof(OOP_TSA_T), "tsa:");
            continue;
        }

        //��ȡ����������
        ret = sta_virtual_ptct_read(&mtrInfo, &ptct);
        if(ret != 0)
        {
            STAT_FMT_DEBUG("sta_virtual_ptct_read failed. ret(%d), mtrType(%d)\n",ret, mtrInfo.type);
            continue;
        }

        if(mtrInfo.type == MTR_PULSE)
        {
            STAT_FMT_DEBUG("[%d]Pn %d %s %s %s PT %d CT %d\n", i, mtrInfo.pulseOAD.nObjID&0xff, mtrichr[mtrInfo.type-1], 
                (tgParam.tg[i].tgFlag==0)?"����":"����", (tgParam.tg[i].opFlag==0)?"��":"��", ptct.pt, ptct.ct);
        }
        else if(mtrInfo.type == MTR_485)
        {
            STAT_FMT_DEBUG("[%d]Pn %d %s %s %s PT %d CT %d\n", i, mtrInfo.nIndex, mtrichr[mtrInfo.type-1], 
                (tgParam.tg[i].tgFlag==0)?"����":"����", (tgParam.tg[i].opFlag==0)?"��":"��", ptct.pt, ptct.ct);
        }

        //ÿһ��ѭ������ʱ��������
        rctPower = 0;
        actPower = 0;

        memset(actEnergy, 0, sizeof(actEnergy));
        memset(rctEnergy, 0, sizeof(rctEnergy));
        
        //��ȡʵʱ��������
        sta_tg_member_power_read(&mtrInfo, &tgParam.tg[i], &actPower, &rctPower);
        //��ȡ���ӵ�������
        sta_virtual_member_min_energy_get(&mtrInfo, &tgParam.tg[i], actEnergy, rctEnergy);

        STAT_FMT_DEBUG("[%d]��Ա���� �й� %lld �޹� %lld\n", i, actPower, rctPower);
        
        STAT_FMT_DEBUG("[%d]��Ա���� �й� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld �޹� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
            i,actEnergy[0],actEnergy[1],actEnergy[2],actEnergy[3],actEnergy[4],
            rctEnergy[0],rctEnergy[1],rctEnergy[2],rctEnergy[3],rctEnergy[4]);
        
        //�й������ܼ�
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, &actPower, &tgActAccPower, 1);
        //�޹������ܼ�        
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, &rctPower, &tgRctAccPower, 1);

        //�й������ܼ�,����Ҫ���������ݺ�������
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActAccEnergy[STA_CALC_DAY], 5);
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActAccEnergy[STA_CALC_MON], 5);
        
        //�޹������ܼ�,����Ҫ���������ݺ�������        
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, rctEnergy, tgRctAccEnergy[STA_CALC_DAY], 5);
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, rctEnergy, tgRctAccEnergy[STA_CALC_MON], 5);            

        //�й����ӵ�����������
        sta_virtual_tgcalc(&ptct, tgParam.tg[i].opFlag, actEnergy, tgActEnergy, 5);

    }

    STAT_FMT_DEBUG("�ܼ���%d���� �й� %lld �޹� %lld\n", tgOAD.nObjID&0xff, tgActAccPower, tgRctAccPower);

    STAT_FMT_DEBUG("�ܼ���%d�յ��� �й� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld �޹� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
        tgOAD.nObjID&0xff, 
        tgActAccEnergy[STA_CALC_DAY][0],tgActAccEnergy[STA_CALC_DAY][1],tgActAccEnergy[STA_CALC_DAY][2],tgActAccEnergy[STA_CALC_DAY][3],tgActAccEnergy[STA_CALC_DAY][4],
        tgRctAccEnergy[STA_CALC_DAY][0],tgRctAccEnergy[STA_CALC_DAY][1],tgRctAccEnergy[STA_CALC_DAY][2],tgRctAccEnergy[STA_CALC_DAY][3],tgRctAccEnergy[STA_CALC_DAY][4]);

    STAT_FMT_DEBUG("�ܼ���%d�µ��� �й� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld �޹� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
        tgOAD.nObjID&0xff, 
        tgActAccEnergy[STA_CALC_MON][0],tgActAccEnergy[STA_CALC_MON][1],tgActAccEnergy[STA_CALC_MON][2],tgActAccEnergy[STA_CALC_MON][3],tgActAccEnergy[STA_CALC_MON][4],
        tgRctAccEnergy[STA_CALC_MON][0],tgRctAccEnergy[STA_CALC_MON][1],tgRctAccEnergy[STA_CALC_MON][2],tgRctAccEnergy[STA_CALC_MON][3],tgRctAccEnergy[STA_CALC_MON][4]);

    STAT_FMT_DEBUG("�ܼ���%d���ӵ��� �й� �� %lld ����1 %lld ����2 %lld ����3 %lld ����4 %lld\n", 
        tgOAD.nObjID&0xff, tgActEnergy[0],tgActEnergy[1],tgActEnergy[2],tgActEnergy[3],tgActEnergy[4]);

    //�洢�ܼӷ��ӵ�����������
    sta_tg_min_acte_write(tgOAD, tgActEnergy);

    //���㻬��ƽ������
    sta_tg_slip_power_calc(tgOAD, tgActAccPower, tgRctAccPower);
    
    //�洢�ܼӹ���
    sta_tg_power_write(tgOAD, tgActAccPower, tgRctAccPower); 
    
    //�洢�ܼӵ���
    sta_virtual_energy_write(tgOAD, tgActAccEnergy, tgRctAccEnergy);     
}

/**
*********************************************************************
* @brief��      �ܼ������
* @param[in]�� tgODA �ܼ������
* @param[in]�� minCalc �ܼ������
* @Create    : ����
* @Date      ��2020-04-17
* @return��     void
*********************************************************************
*/
void sta_tgsets_data_calc(void)
{
    OOP_OAD_U oad = {0};
    uint32 i = 0;
//    uint8 cnt = 0;

//    //�ȴ���������׼��OK
//    while(!pulse_get_ok() && (cnt++ < 20))
//    {
//        usleep(100000);
//    }
//    
//    usleep(100000);
    
    //�˸��ܼ�����ܼӹ��ʺ͵�������
    oad.value = 0x23010200;
    for(i = 0; i < 8; i++)
    {        
        oad.nObjID = 0x2301 + i;

        sta_tg_data_calc(oad);
    }

    return;
}

#endif

#if DESC("���㷨��������",1)
/**
*********************************************************************
* @brief��      ͳ�����ڴ������ṩͳһ������
* @param[in]�� oad       ��������OAD
* @param[out]��interval �������ļ��
* @Create    : ����
* @Date      ��2020-11-26
* @return��     void
*********************************************************************
*/
void sta_period_analyse(OOP_OAD_U oad, OOP_TI_SPAN_E *interval)
{
//    if(oad.nObjID == MT_FROZEN_SEC)
//    {
//        *interval = TI_SEC; 
//    }
//
//    if((oad.nObjID == MT_FROZEN_MIN)||(oad.nObjID == MT_REGION_MIN)
//        ||(oad.nObjID == MT_TOTALAVG_MIN)||(oad.nObjID == MT_EXTREME_MIN))
//    {
//        *interval = TI_MIN;    
//    }
//
//    if((oad.nObjID == MT_FROZEN_HOUR)||(oad.nObjID == MT_REGION_HOUR)
//        ||(oad.nObjID == MT_TOTALAVG_HOUR)||(oad.nObjID == MT_EXTREME_HOUR))
//    {
//        *interval = TI_HOUR;    
//    } 

    if((oad.nObjID == MT_FROZEN_DAY)||(oad.nObjID == MT_REGION_DAY)
        ||(oad.nObjID == MT_TOTALAVG_DAY)||(oad.nObjID == MT_EXTREME_DAY)
        ||(oad.nObjID == MT_FROZEN_SETTLING_DAY) || (oad.nObjID == MT_QR_DAY)
        ||(oad.nObjID == MT_POWERCUT_DAY))
    {
        *interval =TI_DAY;
    }

    if((oad.nObjID == MT_FROZEN_MON)||(oad.nObjID == MT_REGION_MON)
        ||(oad.nObjID == MT_TOTALAVG_MON)||(oad.nObjID == MT_EXTREME_MON)
        ||(oad.nObjID == MT_QR_MON)||(oad.nObjID == MT_POWERCUT_MON))
    {
        *interval = TI_MON;
    }

//    if((oad.nObjID == MT_FROZEN_YEAR)||(oad.nObjID == MT_REGION_YEAR)
//        ||(oad.nObjID == MT_TOTALAVG_YEAR)||(oad.nObjID == MT_EXTREME_YEAR))
//    {
//        *interval = TI_YEAR;    
//    }      
}

/**
*********************************************************************
* @brief��      ���ڼ�������
* @param[in]�� interval ���ڵ�λ
* @param[in]�� period   ����
* @param[out]��count     ���ڼ�����
* @Create    : ����
* @Date      ��2020-11-26
* @return��     1-������ 0-��������
*********************************************************************
*/
BOOLEAN sta_pericnt_update(OOP_TI_SPAN_E span, uint32 period, uint32 *periCnt, uint32 *freqCnt)
{
    BOOLEAN isUpdate = FALSE;

    switch(span)
    {
        case TI_SEC: break;
        case TI_MIN: if(pastminflag)  {(*periCnt)++;}break;
        case TI_HOUR:if(pasthourflag) {(*periCnt)++;}break;
        case TI_DAY: if(pastdayflag)  {(*periCnt)++;}break;
        case TI_MON: if(pastmonflag)  {(*periCnt)++;}break;
        default:break;
    }
    
    //�ﵽ�������������
    if(*periCnt == period)
    {
        STAT_FMT_DEBUG("span(%d), periCnt(%d) == period(%d)\n", span, *periCnt, period);
        
        *periCnt = 0;
        
        if(freqCnt != NULL) 
        {
            *freqCnt = 0;
        }

        isUpdate = TRUE;
    }

    return isUpdate;
}

/**
*********************************************************************
* @brief      ������¼������
* @param[in]  ��clientid   �������ľ��
* @param[in] ��recordOAD ��¼��OAD��Ϣ
* @param[out] ��storeTm    �洢ʱ��
* @param[out] ��outData    ���������
* @param[out] ��outLen     �������������

* @Create     : ����
* @Date       ��2020-11-26
* @return     ��void
*********************************************************************
*/
int sta_record_read(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 *storeTm, void *outData, uint32 outLen)
{
    int ret = ERR_OK;
    READ_RECORD_T readRecord = {{0}};
    RECORD_UNIT_T recordBuf = {0};

    memcpy(&readRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    readRecord.cType = COL_TM_STORE;
    readRecord.sortType = DOWN_SORT;
    readRecord.cEnd = time(NULL) + COMPENSATION_TIME+1; // ע��+8��Сʱ(28800)ת�ɸ���ʱ�䣬�����ǿ�����
    readRecord.cStart = (readRecord.cEnd/60)*60-60;  //ɸѡ��ǰ����1�����ڵ�����
    
    ret = db_read_record(clientid, &readRecord, outLen, &recordBuf);
    if(ret == ERR_OK)
    {
        if(outLen != recordBuf.record[0].outDatalen)
        {
            STAT_FMT_DEBUG("len(%d) != outDatelen(%d), oadMain(0x%08x), oadCols(0x%08x)\n", 
                outLen, recordBuf.record[0].outDatalen, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value);
        }

        *storeTm = recordBuf.colStoreTm;
            
        memcpy(outData, recordBuf.record[0].outDataBuf, outLen);
    }

    return ret;
}


/**
*********************************************************************
* @brief      ������¼������
* @param[in]  ��clientid   �������ľ��
* @param[in] ��recordOAD ��¼��OAD��Ϣ
* @param[out] ��storeTm    �洢ʱ��
* @param[out] ��outData    ���������
* @param[out] ��outLen     �������������

* @Create     : ����
* @Date       ��2020-11-26
* @return     ��void
*********************************************************************
*/
int sta_record_read_new(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 *storeTm, void *outData, uint32 outLen)
{
    int ret = ERR_OK;
    READ_RECORD_T readRecord = {{0}};
    RECORD_UNIT_T recordBuf = {0};

    memcpy(&readRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    readRecord.cType = COL_TM_STORE;
    readRecord.sortType = DOWN_SORT;
    readRecord.cStart = time(NULL) + COMPENSATION_TIME -3600;
    readRecord.cEnd = time(NULL) + COMPENSATION_TIME + 1; // ע��+8��Сʱ(28800)ת�ɸ���ʱ��
    
    ret = db_read_record(clientid, &readRecord, outLen, &recordBuf);
    if(ret == ERR_OK)
    {
        if(outLen != recordBuf.record[0].outDatalen)
        {
            STAT_FMT_DEBUG("len(%d) != outDatelen(%d), oadMain(0x%08x), oadCols(0x%08x)\n", 
                outLen, recordBuf.record[0].outDatalen, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value);
        }

        *storeTm = recordBuf.colStoreTm;
            
        memcpy(outData, recordBuf.record[0].outDataBuf, outLen);
    }

    return ret;
}

/**
*********************************************************************
* @brief��      д��¼������
* @param[in] ��clientid  �������ľ��
* @param[in] ��recordOAD ��¼��OAD��Ϣ
* @param[in] ��storeTm   �洢ʱ��
* @param[in] ��inData     д�������
* @param[in] ��inLen      ���ݳ���

* @Create    : ����
* @Date      ��2020-11-26
* @return    ��void
*********************************************************************
*/
int sta_record_write(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 storeTm, void *inData, uint32 inLen)
{
    WRITE_RECORD_T writeRecord;
    uint32 storeNo;

    MEMZERO(&writeRecord, sizeof(writeRecord));
    memcpy(&writeRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    writeRecord.inDatalen = inLen;
    writeRecord.colStoreTm = storeTm + COMPENSATION_TIME; // ע��+8��Сʱ(28800)ת�ɸ���ʱ��
    writeRecord.colStartTm = writeRecord.colEndTm = writeRecord.colStoreTm;
    
    //����ʱ�����ṹ��ǿת
    memcpy(writeRecord.inDataBuf, (uint8*)inData, inLen);
    
    return db_write_record(clientid, &writeRecord, &storeNo);   
}

/**
*********************************************************************
* @brief��      ���¼�¼������
* @param[in] ��clientid  �������ľ��
* @param[in] ��recordOAD ��¼��OAD��Ϣ
* @param[in] ��storeTm   �洢ʱ��
* @param[in] ��inData     д�������
* @param[in] ��inLen      ���ݳ���

* @Create    : ����
* @Date      ��2020-11-26
* @return    ��void
*********************************************************************
*/
int sta_record_update(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 storeTm, void *inData, uint32 inLen)
{
    UPDATE_RECORD_T updateRecord;

    MEMZERO(&updateRecord, sizeof(updateRecord));
    memcpy(&updateRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    updateRecord.filter.choice = 1;
    updateRecord.inDatalen = inLen;
    updateRecord.filter.storeTm = storeTm;
    
    //����ʱ�����ṹ��ǿת
    memcpy(updateRecord.inDataBuf, (uint8*)inData, inLen);
    
    return db_update_record(clientid, &updateRecord);   
}

#endif

#if DESC("���㷨�����亯��",1)

/**
*********************************************************************
* @brief��      ��ѹ��������
* @param[in]�� clientid  �������ľ��
* @param[in]�� oad        ����ID
* @param[out]��units      ������
* @param[out]��digit      ����������

* @Create    : ����
* @Date      ��2020-11-26
* @return��     void
*********************************************************************
*/
int sta_volt_adapt(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit)
{
    int ret = ERR_OK;
    RECORD_OAD_T recordOAD = {0};
    OOP_WORD3V_T rtData = {0};     //��ǰʵʱ����
    uint32 storeTm = 0;
    int i = 0;

    recordOAD.infoNum = mtrInfo->nIndex;
    recordOAD.road.oadCols.nNum = 1;
    recordOAD.road.oadCols.oad[0].value = oad.value;

    /* ��ȡͳ������Դ */
    ret = sta_record_read(clientid, &recordOAD, &storeTm, (uint8*)&rtData, sizeof(OOP_WORD3V_T));
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("��ȡͳ������Դ failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x).\n",
            ret, recordOAD.logicId, recordOAD.infoNum, recordOAD.road.oadCols.oad[0].value);
    
        return ret;
    }

    *units = rtData.nNum;

    for(i = 0; i < rtData.nNum; i++)
    {
        digit[i].choice = DT_LONG_UNSIGNED;
        digit[i].wVal = rtData.nValue[i];
    }

    return ret;
}

#endif

#if DESC("����ѹ�ϸ���ͳ��",1)

//�ϸ���ͳ�ƣ������������ϸ���ͳ��˼·����ͳ�Ʋ���,����2192,2193Ϊ���ºϸ���ͳ��
STA_QR_OBJ_T g_qr_objs[] = 
{
    /*dstOAD     srcOAD         periCnt  freqCnt  period   freq     upLimit         upRecov  dwLimit   dwRecov  mtAdapt  srcData  nIndex*/ 
    {{0x21920200},  {0x20000200},    0,   0,    {TI_DAY,1},  {TI_MIN,1}, {0}, NULL, 0, NULL, sta_volt_adapt, sta_qr_comm_calc},  //��ѹ
};
const uint32 g_qr_objs_max = sizeof(g_qr_objs)/sizeof(g_qr_objs[0]);

/**
*********************************************************************
* @brief��      �ϸ��ʼ���
* @param[in]�� rtData   ����
* @param[in]�� mtrInfo  ��������Ϣ
* @param[in]�� object   ͳ�ƶ���
* @param[out]: result   ������
* @Create    : ����
* @Date      ��2020-11-26
* @return��     void
*********************************************************************
*/
void sta_qr_calc(OOP_DIGIT_T rtData, STA_MTR_INFO_T *mtrInfo, STA_QR_OBJ_T *object, OOP_QR_RESULT_T *result)
{    
    STAT_FMT_TRACE("qrstat: rtData(%llu), upLimit(%d), upRecov(%d), dwLimit(%d), dwRecov(%d)\n", 
        rtData.ullVal, object->param.upLimit, object->param.upRecov, object->param.dwLimit, object->param.dwRecov);

    //ʱ�䵥λ���Ƿ���
    result->mtTime += object->param.spantime;

    //��������������Ϊ�Ǻϸ��
    if(object->srcValid == FALSE)
    {
        result->mtState = MT_QSTATE_QR;
        result->qrTime += object->param.spantime;
    }
    else
    {
        //����ȫ��ȡ�������
        if((rtData.ullVal > object->param.dwLimit) && (rtData.ullVal < object->param.upLimit))
        {
            result->mtState = MT_QSTATE_QR;
            result->qrTime += object->param.spantime;
        }
        
        //Խ������
        if(rtData.ullVal >= object->param.upupLimit)
        {
            result->mtState = MT_QSTATE_UP;
            result->upupTime += object->param.spantime;
        }
        else 
        {
            //ûԽ�����ޣ��ж��Ƿ�Խ����
            if(rtData.ullVal >= object->param.upLimit)
            {
                result->mtState = MT_QSTATE_UP;
                result->upTime += object->param.spantime;
            }
        }

        //������
        if(rtData.ullVal <= object->param.dwdwLimit)
        {
            result->mtState = MT_QSTATE_DW;
            result->dwdwTime += object->param.spantime;
        }
        else
        {
            //ûԽ�������ж��Ƿ�Խ����
            if(rtData.ullVal <= object->param.dwLimit)
            {
                result->mtState = MT_QSTATE_DW;
                result->dwTime += object->param.spantime;
            }
        }
    }
    
    //����ʱһЩ�ݴ���
    if((pastdayflag)&&(object->period.unit == TI_DAY))
    {
        //������ʱ���ù���ʱ���п���ʱ������1440����
        if(result->qrTime > 1440)
        {
            result->qrTime = 1440;
        }

        if((result->upTime + result->upupTime + result->dwTime + result->dwdwTime) < 1440)
        {
            result->qrTime = 1440-(result->upTime + result->upupTime + result->dwTime + result->dwdwTime);
        }

        result->mtTime = result->upTime + result->upupTime + result->dwTime + result->dwdwTime + result->qrTime;
    }
    
    //�ϸ��ʺ�Խ�����������룬��ȷ��С�����2λ���״ζ�����Ĭ��״̬Ϊ�ϸ�
    result->upRate = (uint16)((float32)(result->upTime + result->upupTime)*10000/result->mtTime + 0.5);   
    result->dwRate = (uint16)((float32)(result->dwTime + result->dwdwTime)*10000/result->mtTime + 0.5);  
    result->qrRate = (uint16)((float32)result->qrTime*10000/result->mtTime + 0.5);    
    
    return;
}

/**
*********************************************************************
* @brief��      �ϸ���ͳ�Ƽ�¼
* @param[in]�� clientid   �������ľ��
* @param[in]�� object      ͳ�ƶ���
* @Create    : ����
* @Date      ��2020-11-26
* @return��     void
*********************************************************************
*/
int sta_qr_comm_calc(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, void *pObject)
{
    int ret = ERR_OK;
    OOP_QR_RESULT_T result[3] = {0};
    STA_QR_OBJ_T *object = (STA_QR_OBJ_T *)pObject;    
    OOP_DIGIT_T *rtData = object->srcData;
    uint32 maxlen = sizeof(result);
    uint32 len = 0;
    NOMAL_OAD_T normalOAD = {0};
    int i = 0;
    RECORD_OAD_T recordOAD = {0};
    time_t curTime = 0;
    OOP_DATETIME_S_T tmpTime = {0};
    OOP_VOLTSTAT_T  voltObj = {0};//��ѹ�ϸ��ʶ���

    memcpy(&tmpTime, &realstattime, sizeof(tmpTime));

    //ͣ�粻�ж�
    if((object->srcValid == TRUE)&&(rtData[0].ullVal == 0)&&(rtData[1].ullVal == 0)&&(rtData[2].ullVal == 0))
    {
        return 0;
    }

    normalOAD.logicId = 0;
    normalOAD.infoNum = mtrInfo->nIndex;
    normalOAD.oad.value = object->dstOAD.value;

    //��ȡ��ʷ����
    ret = db_read_nomal(clientid, &normalOAD, maxlen, (uint8*)result, &len);
    if((ret != ERR_OK) || (len != maxlen))
    {
        STAT_FMT_DEBUG("�ϸ���ͳ��: history read failed. ret(%d) oadMain(0x%08x), infoNum(%d), maxLen(%d)\n", 
            ret, normalOAD.oad.value, normalOAD.infoNum, maxlen);

        //return ret;
    }

    //����ϸ��ʽ��
    for(i = 0; i < 3; i++)
    {
        //����������B�಻ͳ��
        if((mtrInfo->pwrType == PWRTYPE_3PHASE3) && (i == 1))
        {
            //ʱ�䵥λ���Ƿ���
            result[i].mtTime += object->param.spantime;
            result[i].qrRate = 10000;
            continue;
        }
        
        sta_qr_calc(object->srcData[i], mtrInfo, object, &result[i]);

        STAT_FMT_TRACE("\r\n �ϸ���ͳ��: freqCnt(%d): oadMain(0x%08x), oadCols(0x%08x), infoNum(%d)\
                        \r\n mtTime(%d), qrTime(%d) upTime(%d), dwTime(%d)\
                        \r\n upupTime(%d) dwdwTime(%d)\
                        \r\n qrRate(%d), upRate(%d) dwRate(%d)\
                        \r\n mtState(%d)\n", 
                        object->freqCnt, normalOAD.oad.value, object->srcOAD.value, normalOAD.infoNum, 
                        result[i].mtTime,result[i].qrTime,result[i].upTime,result[i].dwTime,
                        result[i].upupTime,result[i].dwdwTime,
                        result[i].qrRate,result[i].upRate,result[i].dwRate,
                        result[i].mtState);
    }
    
    //�ۻ���ɺ��д
    ret = db_write_nomal(clientid, &normalOAD, (uint8*)result, maxlen);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("�ϸ���ͳ��: write failed. ret(%d) oadMain(0x%08x), infoNum(%d), maxLen(%d)\n", 
           ret, normalOAD.oad.value, normalOAD.infoNum, maxlen);
    }

    /* �ﵽͳ�����ڣ��������������ݣ�������ʵʱֵ */    
    if((pastdayflag) && (object->period.unit == TI_DAY))
    {
        recordOAD.road.oadMain.value = 0x50040200;
        tmpTime.hour = 0;
        tmpTime.minute = 0;
        tmpTime.second = 0;

        DT_DateTimeShort2Time(&tmpTime, &curTime);
        recordOAD.infoNum = mtrInfo->nIndex;
        recordOAD.road.oadCols.nNum = 1;
        recordOAD.road.oadCols.oad[0].value = 0x21310200;

        voltObj.day.wRate1 = result[0].qrRate;
        voltObj.day.wRate2 = result[0].upRate+result[0].dwRate;
        voltObj.day.dwTime1 = result[0].mtTime;
        voltObj.day.dwTime3 = result[0].upTime + result[0].upupTime;
        voltObj.day.dwTime4 = result[0].dwTime + result[0].dwdwTime;
        ret = sta_record_write(clientid, &recordOAD, curTime, &voltObj, sizeof(voltObj));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("����: write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
                ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, sizeof(voltObj));
        }
        
        recordOAD.road.oadCols.oad[0].value = 0x21320200;
        
        voltObj.day.wRate1 = result[1].qrRate;
        voltObj.day.wRate2 = result[1].upRate+result[1].dwRate;
        voltObj.day.dwTime1 = result[1].mtTime;
        voltObj.day.dwTime3 = result[1].upTime + result[1].upupTime;
        voltObj.day.dwTime4 = result[1].dwTime + result[1].dwdwTime;
        ret = sta_record_write(clientid, &recordOAD, curTime, &voltObj, sizeof(voltObj));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("����: write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
                ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, sizeof(voltObj));
        }

        recordOAD.road.oadCols.oad[0].value = 0x21330200;
        
        voltObj.day.wRate1 = result[2].qrRate;
        voltObj.day.wRate2 = result[2].upRate+result[2].dwRate;
        voltObj.day.dwTime1 = result[2].mtTime;
        voltObj.day.dwTime3 = result[2].upTime + result[2].upupTime;
        voltObj.day.dwTime4 = result[2].dwTime + result[2].dwdwTime;
        ret = sta_record_write(clientid, &recordOAD, curTime, &voltObj, sizeof(voltObj));
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("����: write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
                ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, sizeof(voltObj));
        }

        //�����¼
        memset(&result, 0, sizeof(result));
        ret = db_write_nomal(clientid, &normalOAD, (uint8*)result, maxlen);

        STAT_FMT_DEBUG("�ϸ���ͳ��: new cycle begin. ret(%d) oadMain(0x%08x), maxLen(%d)\n", 
           ret, normalOAD.oad.value, maxlen);
    }    
    
    return ret;     
}

/**
*********************************************************************
* @name      : sta_qr_dispatch
* @brief     �����ɺϸ���ͳ������
* @param[in] ��clientid   �������ľ��
* @param[in] ��object      ͳ�ƶ���
* @Create    : ����
* @Date      ��2020-11-26
* @return    ��void
*********************************************************************
*/
void sta_qr_dispatch(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, STA_QR_OBJ_T *object)
{
    int ret = ERR_OK;
    OOP_DIGIT_T srcData[128] = {0};
    
    object->srcData = srcData;
    object->srcValid = FALSE;

    //���ݸ�ʽͳһ�ӿ�
    if(object->dataAdapt != NULL)
    {
        ret = object->dataAdapt(clientid, mtrInfo, object->srcOAD, &object->srcUnits, srcData);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("dataAdapt failed. ret(%d), oad(0x%08x)\n", ret, object->srcOAD.value);
        }
        else
        {
            object->srcValid = TRUE;
        }
    }
    else
    {
        STAT_FMT_DEBUG("oad(0x%08x) qr calc not support.\n", object->srcOAD.value);
        return;
    }

    //ͳ�Ƽ���ӿ�
    if(object->calcAdapt != NULL)
    {
        ret = object->calcAdapt(clientid, mtrInfo, object);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("calcAdapt failed. ret(%d), oad(0x%08x)\n", ret, object->srcOAD.value);
            return;
        }
    }
    else
    {
        STAT_FMT_DEBUG("oad(0x%08x) qr calc not support.\n", object->srcOAD.value);
        return;
    }

    return;  
}

/**
*********************************************************************
* @name      : sta_qr_entry
* @brief     ���ϸ���ͳ�����
* @param[in] ��clientid �������ľ��
* @Create    : ����
* @Date      ��2022-11-03
* @return    ��void
*********************************************************************
*/
void sta_qr_entry(DB_CLIENT clientid)
{
    uint32 i = 0, j = 0;
    STA_QR_OBJ_T *tmpNode = NULL;
    OOP_TI_SPAN_E time_span;
    STA_MTR_INFO_T mtrInfo; //��������Ϣ
    OOP_METER_T meter;
    OOP_VOLTPARAM_T voltParam;   //��ѹ�ϸ��ʲ���    
    int alterTime[STA_CALC_MAX] = {0};    //2��Ԫ�أ�0-�գ�1-��
    int ret = 0;

    STAT_FMT_TRACE("****** �ϸ���ͳ�ƿ�ʼ ****** \n");

    for(i = 0; i < g_qr_objs_max; i++)
    {
        tmpNode = &g_qr_objs[i];

        //ͳ�����ڷ���
        sta_period_analyse(tmpNode->dstOAD, &time_span);

        //��ȡ��ѹ�ϸ��ʲ���
        ret = sta_volt_param_read(&voltParam);
        if(ret != 0)
        {
            return;
        }
        
        //�����߹���ʱ��
        sta_volt_monitor_time(alterTime);

        tmpNode->param.upupLimit = voltParam.uupVolt;
        tmpNode->param.upLimit = voltParam.upVolt;
        tmpNode->param.dwdwLimit = voltParam.lldVolt;
        tmpNode->param.dwLimit = voltParam.ldVolt;
        tmpNode->param.spantime = alterTime[0];

        //ͳ�Ƽ�������Ϣ-���ɺ�485��ͳ�ƺϸ���
        for(j = 0; j < oopMeterList.oopMeterNum; j++)
        {
            if(app_get_meter_list_by_id(j, &meter) == 0)
            {
                if(meter.basic.port.nObjID == 0xf201 && 
                    (meter.basic.protocol == PROTO_97 || meter.basic.protocol == PROTO_07))
                {
                        mtrInfo.type = MTR_485;
                }
                else
                {
                    continue;
                }
                
                mtrInfo.nIndex = meter.nIndex;
                mtrInfo.pwrType = meter.basic.pwrType;
                memcpy(&mtrInfo.tsa, &meter.basic.tsa, sizeof(OOP_TSA_T));

                //�ϸ���ͳ�Ʒ��ɺ���
                sta_qr_dispatch(clientid, &mtrInfo, tmpNode);
            }
        }
         
        tmpNode->freqCnt++;

        //ˢ��������
        sta_pericnt_update(time_span, tmpNode->period.value, &tmpNode->periCnt, &tmpNode->freqCnt);           
    }

    STAT_FMT_TRACE("****** �ϸ���ͳ�ƽ��� ****** \n\n");
    
    return;

}

#endif

#if DESC("�ն�ͳ��",1)

/**
*********************************************************************
* @brief��      �ն��ۼƹ���ʱ��(����ͬʱ����)
* @param[in]�� mtrid    �������
* @param[in]�� srcOAD  Դ����OAD
* @param[in]�� dstOAD  Ŀ������OAD
* @return��     void
*********************************************************************
*/
void sta_tmn_power_acctime(void)
{
    uint32 wAccMin[STA_CALC_MAX] = {0}; /*��-0,��-1,�ۼƹ���ʱ�䣨���ӣ�*/
    NOMAL_OAD_T nomalOAD = {0}; 
    uint32 len = 0;
    uint32 ret = 0;
    static int resetflag = 1;
    time_t curTime = 0, lstTime = 0;
    uint32 restartMin = 0;
    OOP_DATETIME_S_T  restartTime = {0};
    int retr = 0;

    nomalOAD.oad.value = 0x22030200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;
    
    /* ��ȡ�ϴ��ն˹���ʱ�� */
    ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(wAccMin), (uint8*)wAccMin, &len);
    
    //������������
    if((pastdayflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past day or ret = %d, clear day power acctime %d->0.\n", ret, wAccMin[STA_CALC_DAY]);
        wAccMin[STA_CALC_DAY] = 0;
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)wAccMin, sizeof(wAccMin));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear day power acctime failed. ret(%d).\n", retr);
        }
    }

    //������������
    if((pastmonflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past mon or ret = %d, clear mon power acctime %d->0.\n", ret, wAccMin[STA_CALC_MON]);
        wAccMin[STA_CALC_MON] = 0;
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)wAccMin, sizeof(wAccMin));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear mon power acctime failed. ret(%d).\n", retr);
        }        
    }

    //�����ۼ�
    if(!pastminflag)
    {
        return;
    }

    //�������һ��ͳ����Ҫ��������ʱ��
    if(resetflag)
    {
        resetflag = 0;
        DT_DateTimeShort2Time(&realstattime, &curTime);
        ret = read_pdata_xram("statpdata",(char *)&restartTime,sizeof(restartTime),sizeof(restartTime));
        DT_DateTimeShort2Time(&restartTime, &lstTime);

        STAT_FMT_DEBUG("ret(%d), offset(%d), restart time(%4d-%02d-%02d %02d:%02d:%02d), sys time(%4d-%02d-%02d %02d:%02d:%02d)\n", 
            ret, sizeof(restartTime), restartTime.year, restartTime.month, restartTime.day, 
            restartTime.hour, restartTime.minute, restartTime.second,
            realstattime.year, realstattime.month, realstattime.day, 
            realstattime.hour, realstattime.minute, realstattime.second);

        restartMin = (curTime-(lstTime/60)*60)/60;

        //����������ʱ�䲻�ܴ���2���ӣ������������������2���ӣ���Ҳ�ǲ����ϲ���Ҫ���
        if(restartMin > 2)
        {
            restartMin = 2;
            STAT_FMT_DEBUG("abnormal restart time, correct to 2 min.\n");
        }
        else if(0 == restartMin)
        {
            /* ��ֹ�ն�����̫�� 0���Ӽ����1min */
            restartMin = 1;
            STAT_FMT_DEBUG("abnormal restart time, correct to 1 min.\n");
        }

        wAccMin[STA_CALC_DAY] += restartMin;
        wAccMin[STA_CALC_MON] += restartMin;

        STAT_FMT_DEBUG("restart used (curTime:%d-(lstTime:%d/60)*60)/60 = %d min\n", curTime, lstTime, restartMin);
    }
    else
    {
        wAccMin[STA_CALC_DAY]++;
        wAccMin[STA_CALC_MON]++;  
    }
        
    if(wAccMin[STA_CALC_DAY] > 1440)  /*�չ�������1440����*/
    {
        STAT_FMT_DEBUG("check day power acctime %d.\n", wAccMin[STA_CALC_DAY]);
        wAccMin[STA_CALC_DAY] = 1440; /*ʱ���޶�����������*/
    }

    if(wAccMin[STA_CALC_MON] > 44640) /*�¹�������44640����*/
    {
        STAT_FMT_DEBUG("check mon power acctime %d.\n", wAccMin[STA_CALC_MON]);
        wAccMin[STA_CALC_MON] = 44640; /*ʱ���޶�����������*/
    }

    //�洢����
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)wAccMin, sizeof(wAccMin));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("tmn power time write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    }
    
    STAT_FMT_TRACE("tmnPowerSupply[day]=%d, tmnPowerSupply[mon]=%d\n", wAccMin[0],wAccMin[1]);

    return;
}


/**
*********************************************************************
* @brief��      �ն��ۼƸ�λ����(����ͬʱ����)
* @param[in]�� mtrid    �������
* @param[in]�� srcOAD  Դ����OAD
* @param[in]�� dstOAD  Ŀ������OAD
* @return��     void
*********************************************************************
*/
void sta_tmn_reset_acctimes(void)
{
    uint16  resetAcc[STA_CALC_MAX] = {0};
    static int resetflag = 1;
    NOMAL_OAD_T nomalOAD = {0}; 
    uint32 len = 0;
    int ret = 0;
    int retr = 0;

    nomalOAD.oad.value = 0x22040200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;

    /* ��ȡ�ϴθ�λ���� */
    ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(resetAcc), (uint8*)resetAcc, &len); 
    if(ret != 0)
    {
        STAT_FMT_DEBUG("read power acctime failed, regarded as reset, ret(%d).\n", ret);
        resetflag = 1; //��ȡ������ͬ��һ������
    }
        
    if((pastdayflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past day or ret = %d, clear day reset acctime %d->0.\n",ret,resetAcc[STA_CALC_DAY]);
        resetAcc[STA_CALC_DAY]  = 0;
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)resetAcc, sizeof(resetAcc));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear day power acctime failed. ret(%d).\n", retr);
        }        
    }
    
    if((pastmonflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past mon or ret = %d, clear mon reset acctime.%d->0\n",ret, resetAcc[STA_CALC_MON]);
        resetAcc[STA_CALC_MON]  = 0; 
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)resetAcc, sizeof(resetAcc));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear mon power acctime failed. ret(%d).\n", retr);
        }        
    }
    
    //δ�����򷵻�
    if(!resetflag) 
    {
        return;
    }
    
    resetAcc[STA_CALC_DAY]++; /*�ո�λ�����ۼ�1*/
    resetAcc[STA_CALC_MON]++; /*�¸�λ�����ۼ�1*/ 
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)resetAcc, sizeof(resetAcc));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("tmn reset acc time write failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 
    
    resetflag = 0;

    STAT_FMT_TRACE("tmnResetTimes[day]=%d, tmnResetTimes[mon]=%d\n", resetAcc[0],resetAcc[1]);  

    return;   
}

extern void comm_flow_get(uint8 bPPP, uint8 index, uint32 *pSendflow, uint32 *pRecvflow);

/**
*********************************************************************
* @brief��      �ն�ͨ������(����ͬʱ����)
* @param[in]�� mtrid    �������
* @param[in]�� srcOAD  Դ����OAD
* @param[in]�� dstOAD  Ŀ������OAD
* @return��     void
*********************************************************************
*/
void sta_tmn_comm_traffic(void)
{
    uint32  traffic[STA_CALC_MAX] = {0};
    uint32 sendFlow = 0;
    uint32 recvFlow = 0; 
    NOMAL_OAD_T nomalOAD = {0}; 
    uint32 len = 0;
    int ret = 0;
    int retr = 0;

    nomalOAD.oad.value = 0x22000200;
    nomalOAD.classtag = CLASS_DATA_NORMAL;

    /* ��ȡ�ϴ�ͨ������ */
    ret =  db_read_nomal(g_staUdpHandle, &nomalOAD, sizeof(traffic), (uint8*)traffic, &len);   
    
    if((pastdayflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past day or ret = %d, clear day comm traffic %d->0.\n",ret,traffic[STA_CALC_DAY]);
        traffic[STA_CALC_DAY]  = 0;
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)traffic, sizeof(traffic));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear day comm traffic failed. ret(%d).\n", retr);
        }           
    }
    
    if((pastmonflag) || (ret != 0))
    {
        STAT_FMT_DEBUG("past mon or ret = %d, clear mon comm traffic %d->0.\n",ret,traffic[STA_CALC_MON]);
        traffic[STA_CALC_MON]  = 0; 
        retr = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)traffic, sizeof(traffic));
        if(retr != 0)
        {
            STAT_FMT_DEBUG("clear mon comm traffic failed. ret(%d).\n", retr);
        }          
    }

    //��ȡ����ͨ������
    comm_flow_get(TRUE, 0, &sendFlow, &recvFlow);
    
    traffic[STA_CALC_DAY] += sendFlow + recvFlow; /*��ͨ�������ۼ�*/
    traffic[STA_CALC_MON] += sendFlow + recvFlow; /*��ͨ�������ۼ�*/ 
    
    ret = db_write_nomal(g_staUdpHandle, &nomalOAD, (uint8*)traffic, sizeof(traffic));
    if(ret != 0)
    {
        STAT_FMT_DEBUG("sta_tmn_comm_traffic write failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inLen(%d)\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, sizeof(uint8));
    
        return;
    } 

    STAT_FMT_TRACE("tmnTraffic[day]=%d, tmnTraffic[mon]=%d\n", traffic[0],traffic[1]);  

    return;
}

#endif


