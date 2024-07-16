/*********************************************
 * Copyright(c) 2007,�Ͼ����������������޹�˾
 * 		All rights reserved.
 * ��������: diproc.h
 * �汾��: 1.1
 * ����: ң������ɼ��ӿ�˵��
 * ����: 
 *********************************************/
#ifndef _DIPROC_H_
#define _DIPROC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define A_FLAG 0xa5             //�л�A���־
#define B_FLAG 0x5a             //�л�B���־
#define C_FLAG 0x84             //�л�C���־
#define SND_FLAG1 0x87          //��Ƶ�ʸ߰�λ��־
#define SND_FLAG2 0x88          //��Ƶ�ʵͰ�λ��־
#define CHECK_A 0x7e            //�л�A��У��ֵ
#define CHECK_B 0x7d            //�л�B��У��ֵ
#define CHECK_C 0x7b            //�л�C��У��ֵ

#define INPUT 1
#define OUTPUT 3

/*
#define SBUS_MAICHONG 1
#define SBUS_YAOXIN   2
#define SBUS_RADIO    3
#define SBUS_FREE     4
*/

/* functions declaration */

/**********************************************
  �������ܵ�Ԫ: ң������ɼ�
  ��������: ��ȡDI״̬�����ص�ǰ��
            ���У�32����DI��״̬
  �������: ��
  �������: ym_status ��ǰ���ɼ����״̬
  ��������ֵ: ������ʾ����0��ʾ�ɹ�
  �ڲ��������̣�ʹ��ϵͳ����read��ȡDI״̬
 **********************************************/
int ReadDi(uint32 *ym_status);
int ReadDiExt(uint32 *ym_status);    //8bit jiaobao 7bit baomi 6bit menjiedian 5bit duangai

/**********************************************
  �������ܵ�Ԫ: ң������ɼ�
  ��������: ��ȡң��״̬�����ص�ǰ��
            ���У�8·��DI��״̬
  �������: ��
  �������: data ��ǰ��·ң��״̬
  ��������ֵ: ������ʾ����0��ʾ�ɹ�
  �ڲ��������̣�ʹ��ϵͳ����read��ȡDI״̬
 **********************************************/
//int ReadYX(uint8 *data);

/**********************************************
  �������ܵ�Ԫ: ң������ɼ�
  ��������: ���̨���ָ����ֵ
            
  �������: data ��д���̨����ֵ
  �������: 
  ��������ֵ: ������ʾ����0��ʾ�ɹ�
  �ڲ��������̣�ʹ��ϵͳ����read��ȡDI״̬
 **********************************************/
int WriteDT(uint8 *data);


//int SBus(char select, uint8 *data);

#ifdef __cplusplus
}
#endif

#endif
