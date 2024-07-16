#ifndef _REMOTE_CU_H
#define _REMOTE_CU_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>


#define   HW_DEVICE_ID_REMOTE_CU  "remote_cu"
#define   REMOTE_CU_LIB_PATH      "/lib/hal_lib/libhal_remote_cu.so"   /* remote_cu ��̬��·�� */

/* ����WATCHDOGģ��ṹ */
typedef struct tag_REMOTE_CU_MODULE 
{
    HW_MODULE common;
}REMOTE_CU_MODULE;

typedef struct tag_REMOTE_CU_DEVICE
{
    struct tag_HW_DEVICE    base;
    /**
    * @brief ����Զ��ͨ�ŵ�Ԫ��Դ
    * @param[in] dev: �豸����
    * @param[in] val: 0���رգ� 1������
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*remote_cu_power_set)(struct tag_REMOTE_CU_DEVICE *dev, uint8 val);
    
    /**
    * @brief ��λԶ��ͨ�ŵ�Ԫ
    * @param[in] dev: �豸����
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*remote_cu_reset)(struct tag_REMOTE_CU_DEVICE *dev);
    
    /**
    * @brief ����Զ��ͨ�ŵ�Ԫ������ƽ
    * @param[in] dev: �豸����
    * @param[in] val: 0��������ƽ�õͣ� 1��������ƽ�ø�
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*remote_cu_on_level_set)(struct tag_REMOTE_CU_DEVICE *dev, uint8 val);
    
    /**
    * @brief ��ȡԶ��ͨ��ģ�鵥Ԫ���״̬
    * @param[in] dev: �豸����
    * @param[out] state: ���״̬��0 δ���룬1 ��ģ������ӦATָ�2 ��ģ�鲻��ӦATָ��
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*remote_cu_insert_state_get)(struct tag_REMOTE_CU_DEVICE *dev, int32 *state);
    
    /**
    * @briefԶ��ͨ��ģ��SIM������
    * @param[in] dev: �豸����
    * @param[in] val: 0���رգ� 1������
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*remote_cu_sim_card_heat)(struct tag_REMOTE_CU_DEVICE *dev, uint8 val);
    
    // �û���չ����

}REMOTE_CU_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

