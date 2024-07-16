#ifndef _KEY_H
#define _KEY_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_KEY  "key"
#define   KEY_LIB_PATH      "/lib/hal_lib/libhal_key.so"   /*key ��̬��·�� */

/* key press */
/*
#define KEY_ESC             1
#define KEY_ENTER             28
#define KEY_UP                 103
#define KEY_LEFT            105
#define KEY_RIGHT           106
#define KEY_DOWN            108
*/

typedef struct tag_KEY_VALUE
{
    uint8 key;    //��׼����
    uint8 status;  //����״̬   0����1  ����
}KEY_VALUE_T;

/* ����KEYģ��ṹ */
typedef struct tag_KEY_MODULE 
{
    HW_MODULE common;
}KEY_MODULE;

/*����KEY�豸�ṹ */
typedef struct tag_KEY_DEVICE
{
    HW_DEVICE    base;
    
    /**
    * @brief ��ȡ���һ�μ�ֵ
    * @param[in] dev: �豸����
    * @param[in] size: ��ȡ��ֵ����󻺳������ȣ� ��λΪ�ֽ�
    * @param[out] key_val_list: ��ֵ�б�����linux�ں˶���ı�׼����ֵ��
    * @note ���������ѭ������ԭ�� �����ӿ�ֻ��ȡ��һ����״ֵ̬�� 
             ����������ϰ���״̬��ƽ̨�������ṩ��
    * @return �ɹ�����ERR_OK��ʧ�ܷ��ش����룬��ֵδ�仯����һ�����ERR_NORMAL����
    */
    int32 (*key_value_get)(struct tag_KEY_DEVICE *dev, struct tag_KEY_VALUE *key_val_list,
    uint32 size);
    
    /**
    * @brief ��ȡ���һ�ΰ���ʱ��ϵͳ����ʱ��
    * @param[in] dev: �豸����
    * @param[out] lst_press_time: ���һ�ΰ���ʱ��ϵͳ����ʱ�䣨ϵͳ����ʱ���ϵͳʱ���Է���ʱӰ�죬��λΪ���롱��
    * @note ����APP��ȡ���һ�ΰ���ʱ��ϵͳ����ʱ���뵱ǰϵͳ����ʱ����бȶԣ�
            ����һ����ֵ������APPǿ���ջ���ʾ����Ȩ��
            ��key_value_get������ȡ����ֵʱˢ��lst_press_time��
            ������洢��/tmp/dev/lst_press_time�ļ��¹����нӿڹ���
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����룬��ֵδ�仯����һ�����
    ��ERR_NORMAL����
    */
    int32 (*key_press_time_get)(struct tag_KEY_DEVICE *dev, uint32 *lst_press_time);


}KEY_DEVICE_T;

#ifdef __cplusplus
}
#endif 


#endif

