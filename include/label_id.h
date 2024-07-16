/*
*********************************************************************
* @filename: label_id.h
* @brief   : ͨ����Ϣlabel����
* @describe: 
* @history: 
*    auther      date          describe
*    ÷����      2020-03-05    create
*********************************************************************
*/
#ifndef    __LABEL_ID_H
#define __LABEL_ID_H

#define LABEL_START    0x3000

/* ͣ�ϵ��¼�͸�� */
#define LABEL_HALT_EVENT                (LABEL_START + 0x106)

//stamr��չģ����ʱ��(label & 0x0f00)Ϊͨ���˿�
/* ͬ��͸���ӿ� */
#define LABEL_HALT_TRANS                (LABEL_START + 0x007)

/* �쳣��͸���ӿ�        ң��   */
#define LABEL_HALT_TRANS_YX             (LABEL_START + 0x008)

/* �쳣��͸���ӿ�        ��Ҫ����   */
#define LABEL_HALT_TRANS_CT             (LABEL_START + 0x009)

/* �쳣��͸���ӿ� ���ӱ���   */
#define LABEL_HALT_TRANS_CN             (LABEL_START + 0x00a)

#endif


