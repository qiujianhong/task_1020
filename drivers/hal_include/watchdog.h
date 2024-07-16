#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>
#include <linux/watchdog.h>


#define   HW_DEVICE_ID_WATCHDOG  "drv_watchdog"
#define   WATCHDOG_LIB_PATH      "/lib/hal_lib/libhal_watchdog.so"   /*watchdog ��̬��·�� */

/* ����WATCHDOGģ��ṹ */
typedef struct tag_WATCHDOG_MODULE 
{
    HW_MODULE common;
}WATCHDOG_MODULE;

/*����WATCHDOG�豸�ṹ */
typedef struct tag_WATCHDOG_DEVICE
{
    HW_DEVICE    base;

    /**
    * @brief ���ÿ��Ź����ʱ��
    * @param[in] dev: �豸����
    * @param[in] period: ��ʱʱ�䣬ȡֵ��Χ[1,3600]����λ����
    * @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
    */
    int (*watchdog_period_set)(struct tag_WATCHDOG_DEVICE *dev, unsigned int period);

    /**
    * @brief ι������
    * @param[in] dev: �豸����
    * @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
    */
    int (*watchdog_clear)(struct tag_WATCHDOG_DEVICE *dev);

    /**
    * @brief ���Ź���λ
    * @param[in] dev: �豸����
    * @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
    */
    int (*watchdog_rst)(struct tag_WATCHDOG_DEVICE *dev);

    /**
    * @brief ��ȡ���Ź�����ʱ��
    * @param[in] dev: �豸����
    * @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
    */
    int (*watchdog_runsec_get)(struct tag_WATCHDOG_DEVICE *dev);
}WATCHDOG_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

