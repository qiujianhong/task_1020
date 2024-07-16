#ifndef __CAN_H
#define __CAN_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h>

#define   CAN0_DEVICE_ID_STR      "can0"
#define   CAN1_DEVICE_ID_STR      "can1"
#define   CAN_LIB_PATH            "/lib/hal_lib/libhal_can.so"   /* can ��̬��·�� */

#define MAX_CAN_NAME_LEN               32

/* ����WATCHDOGģ��ṹ */
typedef struct tag_CAN_MODULE 
{
    HW_MODULE common;
}CAN_MODULE;

typedef struct tag_CAN_DEVICE
{
    struct tag_HW_DEVICE    base;
    
    /**
    * @brief ����CANͨ�Ų���
    * @param[in] dev: �豸����
    * @param[in] baud ������
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼��
    */
    int32 (*can_param_set)(struct tag_CAN_DEVICE *dev, uint32 baud);

    /**
    * @brief ��CAN��������
    * @param[in] dev: �豸����
    * @param[out] buf ���ջ�����
    * @param[in] len ����������
    * @return �ɹ����ش���0�����ݳ��ȣ�ʧ�ܷ��ش����롣�������¼��
    */
    int32 (*can_data_recv)(struct tag_CAN_DEVICE *dev, uint8* buf, uint32 len);
    
    /**
    * @brief ͨ��CAN��������
    * @param[in] dev: �豸����
    * @param[in] buf ���ͻ�����
    * @param[in] len ����������
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼��
    */
    int32 (*can_data_send)(struct tag_CAN_DEVICE *dev, const uint8* buf, int32 len);
    
    /**
    * @brief ��ȡCAN���
    * @param[in] dev: �豸����
    * @return����CAN��ʱ�ľ��ֵ������>0���쳣<=0����
    */
    int32 (*can_get_handle)(struct tag_CAN_DEVICE *dev);
    
    // �û���չ����   
    int32 fd;
    canid_t can_id;
    
    int32  baud;
    char   devName[MAX_CAN_NAME_LEN];
    //int32  canfd;
    uint8* rbuf;
    int32  rlen;
}CAN_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

