
/*
*********************************************************************
* Copyright(C) 2021 China Electric Power Research Institute
* All rights reserved. 
* @brief��   ���Ӳɼ���ز���API�ӿ�
* @date��    2021/6/15
* @history�� 
* @user details�� 
1. ע���ʼ��
    min_gather_init            - ע��ص������������ļ������߳�
2. �����ļ�·�����ն˵�ַ(�����仯��Ҫ��ʱ�ٴε���)
   min_gather_set_file
3. �ָ�APP����֮ǰ���Ķ�������
   (1) min_gather_prepare(TRUE) -  ��ʼ������׼��
   (2) min_gather_input         -  ��ȡ���������������ݣ����ô˺���д��
   (3) min_gather_prepare(FALSE)-  ��ʼ���������
4. ���Ṧ�ܴ򿪹ر�(APP��ʼ����֮���)
   (1) min_gather_frozen_start - ��
   (2) min_gather_input        - д���������ĵĶ������ݵ��ô˽ӿ�д��
   (3) min_gather_frozen_stop  - �ر�
5. ʵʱ���ܴ򿪹ر�(������APP����֮���)
   (1) min_gather_real_start(TRUE) - ��ʵʱ���ݹ��ܴ�(TRUE-��ʼ����ȫ������)
   (2) min_gather_input            - ��ȡ�����������µ�ʵʱ���ݣ����ô˺���д��
   (3) min_gather_real_start(FALSE)- ��ʵʱ���ݹ��ܴ�(FALSE-ȫ�������������)
   (4) min_gather_input            - д���������ĵ�ʵʱ���ݵ��ô˽ӿ�д��
   (5) min_gather_real_stop        - �ر�ʵʱ���ݹ���
6. �ص�����������ݵ�Ԫ�ӿ�
   (1) min_gather_insert_item      - ��������������ݵ�Ԫ
   (2) min_gather_insert_data      - ʵʱ����������ݵ�Ԫ

����˵��:
    ��������SFTPͳһ���ò���������ļ�·������sftp���ӵ��ĸ�Ŀ¼��
    �˿ڣ�822
    �û�����admin
    ���룺admin(����򵥣�ϵͳ��֧��)
(1) �����˿�
    �޸�/etc/ssh/sshd_conig, ����һ�У�����: Port 822
(2) �����û���
    chattr -i /etc/passwd /etc/shadow /etc/group /etc/gshadow
    useradd -d /home/admin -m admin
    chmod 755 -R /home/admin
    passwd admin
*********************************************************************
*/
#ifndef __MIN_GATHER_H
#define __MIN_GATHER_H

#include "cJSON.h"
#include "db_struct.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum
{
    MIN_GATHER_TYPE_REAL     = 0,
    MIN_GATHER_TYPE_MIN      = 0x5002,
    MIN_GATHER_TYPE_HOUR     = 0x5003,
    MIN_GATHER_TYPE_DAY      = 0x5004,
    MIN_GATHER_TYPE_MON      = 0x5006,
}MIN_GATHER_TYPE_E;

typedef enum
{
    MIN_GATHER_OAD_20000201     = 0x20000201, /* A���ѹ uint16 ����:-1 */
    MIN_GATHER_OAD_20000202     = 0x20000202, /* B���ѹ uint16 ����:-1 */
    MIN_GATHER_OAD_20000203     = 0x20000203, /* C���ѹ uint16 ����:-1 */
    MIN_GATHER_OAD_20010201     = 0x20010201, /* A����� int32 ����:-3 */
    MIN_GATHER_OAD_20010202     = 0x20010202, /* B����� int32 ����:-3 */
    MIN_GATHER_OAD_20010203     = 0x20010203, /* C����� int32 ����:-3 */
    MIN_GATHER_OAD_20010400     = 0x20010400, /* ���ߵ��� int32 ����:-3 */
    MIN_GATHER_OAD_20010500     = 0x20010500, /* ʣ����� int32 ����:-3 */
    MIN_GATHER_OAD_20040201     = 0x20040201, /* ���й����� int32 ����:-1 */
    MIN_GATHER_OAD_20040202     = 0x20040202, /* A���й����� int32 ����:-1 */
    MIN_GATHER_OAD_20040203     = 0x20040203, /* B���й����� int32 ����:-1 */
    MIN_GATHER_OAD_20040204     = 0x20040204, /* C���й����� int32 ����:-1 */
    MIN_GATHER_OAD_43140506     = 0x43140506, /* �����¶� int16 ����:-1 */
    MIN_GATHER_OAD_43140505     = 0x43140505, /* �����¶� int16 ����:-1 */
    MIN_GATHER_OAD_43140501     = 0x43140501, /* �յ������¶� int16 ����:-1 */
    MIN_GATHER_OAD_43140400     = 0x43140400, /* �յ����ػ�״̬ int8 */
    MIN_GATHER_OAD_43110502     = 0x43110502, /* ��ˮ�������¶� int16 ����:-1 */
    MIN_GATHER_OAD_43110400     = 0x43110400, /* ��ˮ�����ػ�״̬ int8 */
    MIN_GATHER_OAD_00100201     = 0x00100201, /* �����й��ܵ��� uint32 ����:-2 */
    MIN_GATHER_OAD_00100203     = 0x00100203, /* �����й������ uint32 ����:-2 */
    MIN_GATHER_OAD_00100205     = 0x00100205, /* �����й��ȵ��� uint32 ����:-2 */
    MIN_GATHER_OAD_00200201     = 0x00200201, /* �����й��ܵ��� uint32 ����:-2 */
    MIN_GATHER_OAD_00200203     = 0x00200203, /* �����й������ uint32 ����:-2 */
    MIN_GATHER_OAD_00200205     = 0x00200205, /* �����й��ȵ��� uint32 ����:-2 */
}MIN_GATHER_OAD_E;

/**
*********************************************************************
* @brief��     �������ݴ�����
* @param[in]�� int             cid       �������Ŀͻ���
               uint16          type      ����(MIN_GATHER_TYPE_E)
               WRITE_RECORD_T *pData     д������
               uint32          intv      ���ݼ��
               void*           pGdate    �ɼ�����
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
typedef int (*FROZEN_DATA_FUN)(int cid, uint16 type, WRITE_RECORD_T*, uint32, void*);

/* ������ */
typedef struct tagMinGatherFun
{
    uint32           OAD;
    FROZEN_DATA_FUN  func;
}MIN_GATHER_FUN_T;

//int froen_real_00100200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
//{
//    OOP_ENERGY_T ENERGY_T;
//    
//    if(MIN_GATHER_TYPE_REAL == type)
//    {
//        memcpy(ENERGY_T,pData->inDataBuf,pData->inDatalen);
//        min_gather_insert_data(pGdate, MIN_GATHER_OAD_00100201, pData->colStoreTm, &ENERGY_T.nValue[0], sizeof(uint32));
//    }
//
//    else
//    {
//        cJSON *jData_00100201 = NULL;
//        cJSON *jData_00100202 = NULL;
//        jData_00100201 = cJSON_CreateNumber(0);
//        jData_00100202 = cJSON_CreateNumber(0);
//        min_gather_insert_item(pGdate, type, 0x00100201, pData->colStoreTm, pData->colEndTm, intv, jData_00100201);
//        min_gather_insert_item(pGdate, type, 0x00100201, pData->colStoreTm, pData->colEndTm, intv, jData_00100202);
//    }
//    return 0;
//}
//MIN_GATHER_FUN_T g_Frozen[]
//{
//    {0x00100200, froen_real_00100200},
//};
//uint32 g_FrozenNUm = sizeof(g_Frozen)/sizeof(MIN_GATHER_FUN_T);
//
//MIN_GATHER_FUN_T g_Real[]
//{
//    {0x00100200, froen_real_00100200},
//    {0x00200200, froen_real_00200200},
//    {0x20000200, froen_real_20000200},
//    {0x20010200, froen_real_20010200},
//    {0x20010400, froen_real_20010400},
//    {0x20010500, froen_real_20040200},
//    {0x20040200, froen_real_20040200},
//};
//uint32 g_RealNUm = sizeof(g_Real)/sizeof(MIN_GATHER_FUN_T);

/**
*********************************************************************
* @brief��     ���Ӳɼ���ʼ��
* @param[in]�� char *filedir             �ļ��洢·��
               MIN_GATHER_FUN_T *frofun  �������ݴ�����
               uint32 fronum             �������ݴ���������
               MIN_GATHER_FUN_T *realfun ʵʱ���ݴ�����
               uint32 realnum            ʵʱ���ݴ���������
               uint8  timemin            ����ʱ��(0��timemin��, ���59)
               uint8  zipmin             ѹ��ʱ��(0��zipminѹ����zipmin > timemin, 0��Ч�����60)
               
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int min_gather_init(MIN_GATHER_FUN_T *frofun, uint32 fronum, MIN_GATHER_FUN_T *realfun, uint32 realnum, uint8 timemin, 
                    uint8 zipmin);

/**
*********************************************************************
* @brief��     �����ļ�·��
* @param[in]�� char *filedir             �ļ��洢·��
               uint8 *addr               �ն˵�ַ(6���ֽ�)
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
void min_gather_set_file(char *filedir, uint8 *addr);

/**
*********************************************************************
* @brief��     �����ڲɼ���������(����APP�״�������, ���������Ļ�ȡ���ݺ���������JSON����)
* @param[in]�� uint8 flag    1-��ʼ(�����������������ݶ���������min_gather_inputд��); 0-ֹͣ
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int min_gather_prepare(uint8 flag);

/**
*********************************************************************
* @brief��     �����ڲɶ��Ὺʼ
* @param[in]�� 
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int min_gather_frozen_start();

/**
*********************************************************************
* @brief��     �����ڲɶ���ֹͣ
* @param[in]�� uint8 bstore   �Ƿ���Ҫ�����Ѿ����ڵ�����
* @param[out]��
* @return��    
*********************************************************************
*/
void min_gather_frozen_stop(uint8 bstore);

/**
*********************************************************************
* @brief��     ���Ӳɼ�ʵʱ���ݲɼ���ʼ
* @param[in]�� uint8 bAllFlag     ȫ�����ɱ��(TRUE-����ȫ�����, FALSE-ȫ��������)
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int min_gather_real_start(uint8 bAllFlag);

/**
*********************************************************************
* @brief��     ���Ӳɼ�ʵʱ���ݲɼ�����
* @param[in]�� 
* @param[out]��
* @return��    
*********************************************************************
*/
void min_gather_real_stop();

/**
*********************************************************************
* @brief��     ���Ӳɼ���������
* @param[in]�� int cid                   �������Ŀͻ���
               WRITE_RECORD_T *pData     д������
               uint32 intv               ���ݼ��(���磬15���Ӳɼ�����д15)
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int min_gather_input(int cid, WRITE_RECORD_T *pData, uint32 intv);

/**
*********************************************************************
* @brief��     ���Ӳɼ����Ӷ�������
* @param[in]�� void *pGdate              �ɼ�����
               uint16 type               ����(MIN_GATHER_TYPE_E)
               uint32 itemId             ���ݱ�ʶ
               uint32 storeTm            �洢ʱ��
               uint32 succTm             �ɹ�ʱ��
               uint32 intv               ���
               cJSON *jsonData           ��������
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int min_gather_insert_item(void *pGdate, uint16 type, uint32 itemId, uint32 storeTm, uint32 succTm, uint32 intv, 
                           cJSON *jsonData);

/**
*********************************************************************
* @brief��     ���Ӳɼ����ʵʱ����
* @param[in]�� void *pGdate              �ɼ�����
               uint32 type               ���ݱ�ʶ(MIN_GATHER_OAD_E)
               uint32 storeTm            �洢ʱ��
               uint8 *data               ��������
               uint16 datalen            ���ݳ���
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int min_gather_insert_data(void *pGdate, uint32 type, uint32 storeTm, uint8 *data, uint16 datalen);

#ifdef __cplusplus
}
#endif

#endif

