#ifndef _DRC_CLOSING_H
#define _DRC_CLOSING_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>


#define HW_DEVICE_ID_DRV_CLOSING  "drv_closing"
#define DRV_CLOSING_LIB_PATH      "/lib/hal_lib/libhal_drv_closing.so"   /* drv_closing ��̬��·�� */

/* ����WATCHDOGģ��ṹ */
typedef struct tag_DRV_CLOSING_MODULE 
{
    HW_MODULE common;
}DRV_CLOSING_MODULE;

typedef enum //<��բģʽ
{
    CLOSING_PULSE = 0,
    CLOSING_LEVEL,
}CLOSING_MODE_E;

typedef enum //<��������
{
    CLOSING_TURN_ON = 0,
    CLOSING_TURN_OFF,
}CLOSING_TURN_E;


typedef struct tag_CLOSING_DEVICE
{
    struct tag_HW_DEVICE    base;
    
    /**
    * @brief ���ú�բ�̵������ģʽ
    * @param[in] dev: �豸����
    * @param[in] mode: ��բģʽ
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*closing_mode_set)(struct tag_CLOSING_DEVICE *dev, CLOSING_MODE_E mode);
    
    /**
    * @brief ���ú�բ�̵������������
    * @param[in] dev: �豸����
    * @param[in] period: ��բ�̵�����������ȣ���λ��ms
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*closing_pulse_width_set)(struct tag_CLOSING_DEVICE *dev, uint32 pulse_width);

    /**
    * @brief ��ȡ��բ����״̬
    * @param[in] dev: �豸����
    * @param[out] state: ����״̬��bit0��bit(n-1)��λ��ʾ1~n �ֽ���״̬��1�н��ߣ�0 û����
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*closing_line_state_get)(struct tag_CLOSING_DEVICE *dev, uint32 *state);
    
    /**
    * @brief��բ�̵����������
    * @param[in] dev: �豸����
    * @param[in] id: 1~4Ϊ4�ֺ�բ��5Ϊ�澯�����������ͬ�������
    * @param[in] turn: ��������
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*closing_turn)(struct tag_CLOSING_DEVICE *dev, uint8 id, CLOSING_TURN_E turn);

    /**
    * @brief�澯�̵����������
    * @param[in] dev: �豸����
    * @param[in] turn: ��������
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*closing_turn_alarm)(struct tag_CLOSING_DEVICE *dev, CLOSING_TURN_E turn);
    
    // �û���չ����
    int   fd;
    uint8 index;

    CLOSING_MODE_E mode;
    uint32         pulse_width;
    void          *list;
    pthread_t      threadid;
    uint8          stop_flag;

    uint8          turn[4];
}DRV_CLOSING_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

