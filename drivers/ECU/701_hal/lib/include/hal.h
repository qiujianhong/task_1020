#ifndef _HAL_H
#define _HAL_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/file.h> 
#include <fcntl.h>
#include <errno.h>
#include <termios.h> 

#include "types.h"
#include "sysDefine.h"

#define   HAL_MODULE_INFO_SYM   HMI            /* HAL module���� */
#define   HAL_MODULE_INFO_SYM_AS_STR  "HMI"    /* HAL module�����ַ��� */

#define HW_MODULE_MAXDEVICES (128)    /* һ��ģ���ڰ���������豸���� */
#define HW_DEVICE_ID_MAXLEN  (32)    /*�豸ID����󳤶� */

struct tag_HW_DEVICE;
struct tag_HW_MODULE;

typedef struct tag_HW_MODULE 
{ 
    const char *szName;       /* ģ���� */
    unsigned int nVer;        /* ģ��汾�� */
    const char *szAuthor;     /* ���� */
    unsigned int nFactory;    /* ���̴��� */
    const char *szBuildDate;  /* ��������YYYY-MM-DD */
    uint32 nDevNum; /* ģ�������Ӳ���豸�� */
    char ** szDevices;  /* ģ�������Ӳ���豸���� */

    /**
    * @brief ���豸���ʽӿ�
    * @param[in] pModule: �ӿ�ģ�����ָ��
    * @param[in] szDeviceID: �豸ID 
    * @param[in] ppDevice: �豸���ʽӿڶ���ָ��
    * @return �ɹ�����module�ṹָ�룬ʧ�ܷ���NULL
    */
    int (*pfOpen)( struct tag_HW_MODULE *pModule, const char *szDeviceID, 
        struct tag_HW_DEVICE **pDevice ); 
    
    /**
    * @brief �ر��豸���ʽӿ�
    * @param[in] pModule: �ӿ�ģ�����ָ��
    * @param[in] ppDevice: �豸���ʽӿڶ���ָ��
    * @return �ɹ�����0
    */
    int (*pfClose)(struct tag_HW_MODULE *pModule, struct tag_HW_DEVICE *pDevice ); 
}HW_MODULE;

typedef struct tag_HW_DEVICE 
{ 
    HW_MODULE *pModule;         /* �豸����ģ�����ָ�� */
    int nVer;                   /* �豸�ӿڰ汾�� */
    const char * szDeviceID;  /* �豸ID�� */
}HW_DEVICE;


/****************************************************
* @brief ΪӦ�ò��ṩ��Ӳ��������ڣ���ȡ�豸
* @param[in] device_id: ָ��device id
*****************************************************/
 struct tag_HW_DEVICE* hal_device_get(const char *device_id);


/**
*********************************************************************
* @brief       �ر��豸���ͷ��豸����
* @param[in]  dev: �豸ָ��
* @param[out]  ��
* @return      �ɹ�����SUCCEED; ʧ�ܷ��ش�����
* @note        ������Ӧ�ñ���ֲᣨ��ϸ����������ʹ��˵����
* @warning     ע������
*********************************************************************
*/
int hal_device_release(struct tag_HW_DEVICE* dev);


/**
*********************************************************************
* @brief       hal���ʼ��
* @param[in]   ��
* @param[out]  ��
* @return      �ɹ�����0; ʧ�ܷ���С��0������
* @note        ������Ӧ�ñ���ֲᣨ��ϸ����������ʹ��˵����
* @warning     ע������
*********************************************************************
*/
extern int hal_init(void);

/**
*********************************************************************
* @brief       �������˳� 
* @param[in]   ��
* @param[out]  ��
* @return      �ɹ�����0; ʧ�ܷ���С��0������
* @note        ������Ӧ�ñ���ֲᣨ��ϸ����������ʹ��˵����
* @warning     ע������
*********************************************************************
*/
extern int hal_exit(void);

#ifdef __cplusplus
}
#endif 

#endif
