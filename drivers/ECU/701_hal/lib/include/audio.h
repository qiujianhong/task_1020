#ifndef _AUDIO_H
#define _AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_AUDIO  "audio"
#define   AUDIO_LIB_PATH      "/lib/hal_lib/libhal_audio.so"  /*audio ��̬��·�� */

typedef enum
{
    EMAUDIO_INDEX_0 = 0,  //�ر���Դ���
    EMAUDIO_INDEX_1,      //���в����Ѹ��ģ���ע��鿴
    EMAUDIO_INDEX_2,      //������Ϣ����鿴 
    EMAUDIO_INDEX_3,      //ʱ�ο���Ͷ�� 
    EMAUDIO_INDEX_4,      //ʱ�ο��ѽ�� 
    EMAUDIO_INDEX_5,      //���ݿ���Ͷ�� 
    EMAUDIO_INDEX_6,      //���ݿ��ѽ�� 
    EMAUDIO_INDEX_7,      //Ӫҵ��ͣ��Ͷ�� 
    EMAUDIO_INDEX_8,      //Ӫҵ��ͣ�ؽ�� 
    EMAUDIO_INDEX_9,      //�µ����Ͷ�� 
    EMAUDIO_INDEX_10,     //�µ���ѽ��
    EMAUDIO_INDEX_11,     //�������Ͷ�� 
    EMAUDIO_INDEX_12,     //������ѽ�� 
    EMAUDIO_INDEX_13,     //��ǰ�����¸�����Ͷ�� 
    EMAUDIO_INDEX_14,     //��ǰ�����¸����ѽ�� 
    EMAUDIO_INDEX_15,     //ң����բ 
    EMAUDIO_INDEX_16,     //�����բ 
    EMAUDIO_INDEX_17,     //�ѳ����ɣ����޵� 
    EMAUDIO_INDEX_18,     //���������꣬���ٹ��� 
    EMAUDIO_INDEX_19,     //�������꣬����բ 
    EMAUDIO_INDEX_20,     //��(������,1��) 
    EMAUDIO_INDEX_21,     //���û���ʱ������
    EMAUDIO_INDEX_22,     //������բ
    EMAUDIO_INDEX_23,     //�ƿ���բ
    EMAUDIO_INDEX_24,     //�� �� ��
}AUDIO_INDEX_E;

/* ����AUDIOģ��ṹ */
typedef struct tag_AUDIO_MODULE 
{
    HW_MODULE common;
}AUDIO_MODULE;

/*����AUDIO�豸�ṹ */
typedef struct tag_AUDIO_DEVICE
{
	HW_DEVICE	base;

	/**
	* @brief ������Ƶ�����豸
	* @param[in] dev: �豸����
	* @param[in] state: 0 ��ֹ����,1 ʹ�ܲ���
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	int32 (*audio_state_set)(struct tag_AUDIO_DEVICE *dev, uint32 state);
	/**
	* @brief ���ŵ�N����Ƶ
	* @param[in] dev: �豸����
	* @param[in] index: ��Ƶ���
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	int32 (*audio_play)(struct tag_AUDIO_DEVICE *dev, AUDIO_INDEX_E index);
	/**
	* @brief ���������
	* @param[in] dev: �豸����
	* @param[in] val: 0�� �رգ� 1�� ����
	* @param[in] frq: ���Ƶ�ʣ���λ: hz��
	* @param[in] count: ����
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	int32 (*audio_set_buzzer_out)(struct tag_AUDIO_DEVICE *dev, uint8 val, uint32 frq, uint32 count);	
	// �û���չ����	
}AUDIO_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

