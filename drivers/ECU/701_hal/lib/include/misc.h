#ifndef _MISC_H
#define _MISC_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_MISC  "misc"
#define   MISC_LIB_PATH      "/lib/hal_lib/libhal_misc.so"   /*misc ��̬��·�� */

typedef enum
{
    EMFS_TYPE_UBIFS = 0, //ubifs
    EMFS_TYPE_JIFFS2 = 1, //jiffs2
    EMFS_TYPE_MAX = 255,
}FS_TYPE_E;

/* ����MISCģ��ṹ */
typedef struct tag_MISC_MODULE 
{
    HW_MODULE common;
}MISC_MODULE;

/*����MISC�豸�ṹ */
typedef struct tag_MISC_DEVICE
{
	HW_DEVICE	base;

/**
* @brief ��ȡ�ļ�ϵͳ����״̬
* @param[in] dev: �豸����
* @param[in] fs_type: �ļ�ϵͳ���ͣ���ubifs/jiffs2�ȣ�
* @param[in] partition_idx:�����������
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 (*misc_fs_partition_state_get)(struct tag_MISC_DEVICE *dev, int32 fs_type, int32 partition_idx);
/**
* @brief ����pwm����/�ر�
* @param[in] dev: �豸����
* @param[in] val: ����/�ر�����ֵ
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 (*misc_pwm_state_set)(struct tag_MISC_DEVICE *dev, int32 val);
/**
* @brief ����pwm����
* @param[in] dev: �豸����
* @param[in] frq: pwmƵ��
* @param[in] duty: pwmռ�ձ�
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 (*misc_pwm_param_set)(struct tag_MISC_DEVICE *dev, uint32 frq, uint32 duty);
/**
* @brief ��ȡpwm����
* @param[in] dev: �豸����
* @param[out] frq: pwmƵ��
* @param[out] duty: pwmռ�ձ�
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 (*misc_pwm_param_get)(struct tag_MISC_DEVICE *dev, uint32 *frq, uint32 *duty);
/**
* @brief ��ȡUSB������Ϣ
* @param[in] dev: �豸����
* @param[out] info: ��ȡ��Ϣ
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 (*misc_usb_info_get)(struct tag_MISC_DEVICE *dev, uint32 *info);
/**
* @brief sim�����ȹ���
* @param[in] dev: �豸����
* @param[in] val: 0�� ֹͣ���ȣ� 1�� ��������
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 (*misc_sim_heater_set)(struct tag_MISC_DEVICE *dev, uint8 val);
// �û���չ����  
}MISC_DEVICE_T;

#ifdef __cplusplus
}
#endif 


#endif

