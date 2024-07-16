#ifndef _POWER_H
#define _POWER_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_POWER  "power"
#define   POWER_LIB_PATH      "/lib/hal_lib/libhal_power.so"   /*power ��̬��·�� */

typedef enum        ///<У��
{
POWER_MAIN = 0,
POWER_BACKUP,
} POWER_ID_E;

typedef enum        ///<У��
{
USB_NO = 0,
MODULE_1,
MODULE_2,
MODULE_3,
MODULE_4,
MODULE_5,
MODULE_BT,
MODULE_USB_HUB,
} MODULE_ID_E;

/* ����pwerģ��ṹ */
typedef struct tag_POWER_MODULE 
{
    HW_MODULE common;
}POWER_MODULE;

/*����power�豸�ṹ */
typedef struct tag_POWER_DEVICE
{
	HW_DEVICE	base;
    
	/**
	* @brief ��ȡ��Դ״̬
	* @param[in] dev: �豸����
	* @param[out] state: ��Դ״̬��1�е磬0 ����
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	int32 (*power_power_state_get)(struct tag_POWER_DEVICE *dev, uint8 power_id, int32 *state);
	/**
	* @brief ���õ�Դ������
	* @param[in] dev: �豸����
	* @param[in] type: 0�� ���õ�أ� 1�� ��������
	* @param[in] val: 0�� ����磬 1�� ���
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	int32 (*power_battery_charge_set)(struct tag_POWER_DEVICE *dev, uint8 type, uint8 val);
	/**
	* @brief ���õ�Դ�������
	* @param[in] dev: �豸����
	* @param[in] type: 0�� ���õ�أ� 1�� ��������
	* @param[in] val: 0�� �رչ��磬 1�� ����
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	int32 (*power_battery_out_set)(struct tag_POWER_DEVICE *dev, uint8 type, uint8 val);
	/**
	* @brief ��չģ���Դ����
	* @param[in] dev: �豸����
	* module_id 	  // 5��ģ��+1������+usb hub	
	* @param[in] val: 0�� �رգ� 1�� ����
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	int32 (*power_ply_vcc_out_set)(struct tag_POWER_DEVICE *dev,uint8 module_id, uint8 val);
	/**
	* @brief ��չģ���Դ����
	* @param[in] dev: �豸����
	* module_id       // 5��ģ��+1������+usb hub  
	* @param[in] val: 0�� �رգ� 1�� ����
	* @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
	*/
	int32 (*power_ply_vcc_state_get)(struct tag_POWER_DEVICE *dev,uint8 module_id, int32 *state);

	// �û���չ����	
}POWER_DEVICE_T;



#ifdef __cplusplus
}
#endif 

#endif

