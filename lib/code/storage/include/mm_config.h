/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ����������������޹�˾
             All rights reserved.

  ��������: utils.c
  ��    �ܣ��洢��ϵͳ����
  �� �� �ˣ�zhangyu
  ����ʱ�䣺2006-11-06

  �汾��ʷ:

  2006-11-06 :
    v1.0, zhangyu, �½�

***************************************************************************
*/

#ifndef __MM_CONFIG_H__
#define __MM_CONFIG_H__

#include "../../include/dataType.h"

#define EEPROM_SIM_EN 0 /* eeprom�洢��ģ�⿪��:
                         *  1-���ļ�ģ��eeprom,
                         *  0-��дʵ��eeprom(��ARM�ն�, PC��ʼ��ʹ���ļ�ģ��)
                         */

#define MEM_ERR_LOGFILE "/mnt/log/mem.log"

#define PROJECTID   0     //�ͻ�(����)���
#define MAJOR       1     //���汾�ţ��ڶԶ�̬�⹦��ʵ���ش�Ķ�����ģ����磺
                          //�ӿں������ش�仯�������ڲ�ʵ�ֻ������ش�ı�ȵȡ�
#define MINOR       0     //�ΰ汾�ţ�һ������Զ�̬���ڲ�ʵ����С��С������Ը��ġ�

/* 8k eeprom�ռ�ʹ�÷ֲ�:
   =========================

        �ն����м����ܲ���
              (����)              ������;(AC)    г��HR   eeprom˽��������
 ����Mac��ַ     |                    |              |           |
    /\   ________|_______   __________|__________   _|__   ______|______
   /  \ /                \ /                     \ /    \ /             \
  |----|--------^---------|-----------------------|------|---------------|
    1  +   3k - 128Bytes            3k            + 256  +  2k-256Bytes
    2   \                                                   \
    8    \                                               \
    B     \--XPARABASE                                    \--PDATABASE
*/
#define   XPARABASE         0x80      //eeprom��������ʼ�ֽڵ�ַ
//#define   PDATABASE       0x1900    //˽����������EEPROM�еĻ���ַ
#define   PDATABASE         0x1A00    //˽����������EEPROM�еĻ���ַ


#define   EEPROM_SIZE_TOTAL 0x2000    //eeprom����, 8k
//#define   REG_MAX_LEN       0x0c00    //eeprom���ע�᳤��, 2k
#define   REG_MAX_LEN       0x0B00    //eeprom���ע�᳤��, 2k-256Bytes

#define   XPARACHK_SIZE     1         //�����������У���ֽ���
#define   XPARA_CHECKTYPE   1         //У������, 1- XPARA_SUMCHECK, 2- XPARA_CRC16, 3- XPARA_CRC32

#define   SD_MAX_MONTHS     48        //SD��������ŵ�������Ŀ¼��

#define   ERASE_CHAR        0xff      // �����ַ��������������

#ifdef ARM
  #define   RAMDISK_DEVID     0x0e      //ramdisk���豸��, 0x0e��Ŀǰ�ն˵�ramdisk��
  #define   SDCARD_DEVID      0x802     //sdcard���豸��
#else
  #define   RAMDISK_DEVID     0x100     //ramdisk���豸��
  #define   SDCARD_DEVID      0x802     //sdcard���豸��
#endif

#define   XPARASHM_KEYID    100       //����ӳ�����ݷ���ͬ����Ϣ�����ڴ���key
#define   XMTRSHM_KEYID     102       //����ӳ�����ݷ���ͬ����Ϣ�����ڴ���key
#define   XPARASEM_KEYID    101       //����ӳ�����ݷ���ͬ����Ϣ�ź���key

#define   PATH_MAX_LEN      256       //����·����󳤶�

#define   XPARA_DEL_MINID  JLM_LOCATION + 1 //����ն����в���ʱ����СID��

#define   LOG_MAX_NUM       10000      //��־�ļ��������

/*
 * �ն˲���������Ϣ(��Ҫ������eeprom�еĲ�������)
 */

// ϵͳΪ�ն˱����˴��3KB���ҵ�eeprom�ռ��������沿���ն����м����ܲ���,
// ������ȡ�����ֲ�����ǰ��һ�������ݱ��浽eeprom�У�����������Ϊ�����ֲ�
// �������ֽڿռ䲢����һ���Ŀռ�ԣ��. ��������Ҫ���???PARA_EEPROM_MAXID,
// ������qtables.c�ļ��еĶ�Ӧ�������ݵ���(��֤�ܹ��ŵ��¾���)!
//
#define   RUNPARA_SIZE   1000      //���в���ռ���ֽ���
#define   FUNCPARA_SIZE  0          //���ܲ���ռ���ֽ���

#define   RUNPARABASE  XPARABASE  // ���в���eeprom����ַ
#define   FUNCPARABASE RUNPARABASE + RUNPARA_SIZE  // ���ܲ���eeprom����ַ

#define   RUNPARA_EEPROM_MAXID    JLM_LOCATION      //��EEPROM�б�����ն����в��������ID��
#define   FUNCPARA_EEPROM_MAXID Tariff_SEG_ID         //��EEPROM�б�����ն˹��ܲ��������ID��

#endif //__mm_config_h__
