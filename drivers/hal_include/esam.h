#ifndef _ESAM_H
#define _ESAM_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define HW_DEVICE_ID_YX_ESAM "yx_esam" /* Ӫ��esam*/
#define HW_DEVICE_ID_PW_ESAM "pw_esam" /* ����esam*/


#define ESAM_LIB_PATH      "/lib/hal_lib/libhal_esam.so"  /*esam ��̬��·�� */ 

/*��Դ��ģ�� */
typedef enum
{
    DEV_SWITCH=2,
    DEV_NET=4,
    DEV_USBNET=5,
    ESAM_PWR=6,
}ESAM_SUBDEV;

/*����״̬��0 �رգ�1 �� */
typedef enum
{
    PWR_OFF=0,
    PWR_ON=1,
}ESAM_PWR_STATE;

typedef enum
{
    EM_ESAM_YX = 0,
    EM_ESAM_PW,
} EM_ESAM_USR;

/* ����ESAMģ��ṹ */
typedef struct tag_ESAM_MODULE 
{
    HW_MODULE common;
}ESAM_MODULE;

/*����ESAM�豸�ṹ */
typedef struct tag_ESAM_DEVICE
{
    HW_DEVICE    base;
    
    /**
    * @brief ����esam��λ
    * @param[in] dev: �豸����
    * @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
    */
    int32 (*esam_set_reset)(struct tag_ESAM_DEVICE *dev);
    
    /**
    * @brief ����esam��Դ״̬ 1������ 0���ر�
    * @param[in] dev: �豸����
    * @param[in] val: ��Դ״̬
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*esam_set_power)(struct tag_ESAM_DEVICE *dev, int32 val);
    
    /**
    * @brief ����esam�豸����
    * @param[in] dev: �豸����
    * @param[in] val: 1Ϊ����  0 Ϊ����
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*esam_set_lock)(struct tag_ESAM_DEVICE *dev, int32 val);
    
    /**
    * @brief ��ȡesam����
    * @param[out] buf: ��ȡ���ݻ���
    * @param[in] len: ��ȡ���ݳ���
    * @return�ɹ����ض��������ݳ��ȣ�ʧ�ܷ��ش����롣
    */
    int32 (*esam_data_read)(struct tag_ESAM_DEVICE *dev, uint8 *buf, int32 len);
    
    /**
    * @briefдESAM����
    * @param[in] dev: �豸����
    * @param[in] buf: д���ݻ���
    * @param[in] len: д���ݳ���
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*esam_data_write)(struct tag_ESAM_DEVICE *dev, uint8 *buf, int32 len);
    // �û���չ����    
    int32 fd_lock;
	int32 fd;
    EM_ESAM_USR             type;                   ///< 0:xesam 1:pesam
    pthread_mutex_t         devMetux;
}ESAM_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

