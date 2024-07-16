#ifndef __XL_M_PLATFORM_H__
#define __XL_M_PLATFORM_H__

#include "baseType.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief:ƽ̨ģ����ʼ�� */
sint32 platform_init_system(void);

/* @brief:ƽ̨ģ������ */
void platform_clean_up_system(void);

/* @brief:ƽ̨ģ������ */
sint32 platform_start_system(void);

/* @brief:ƽ̨ģ��ֹͣ */
void platform_stop_system(void);

#ifdef __cplusplus
}
#endif

#endif