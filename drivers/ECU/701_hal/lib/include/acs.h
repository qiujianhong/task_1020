#ifndef _ACS_H
#define _ACS_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_ACS  "acs"
#define   ACS_LIB_PATH      "/lib/hal_lib/libhal_acs.so"   /* */

/* ����ACSģ��ṹ */
typedef struct tag_ACS_MODULE 
{
    HW_MODULE common;
}ACS_MODULE;

/*����PULSE�豸�ṹ */
typedef struct tag_ACS_DEVICE
{
	HW_DEVICE	base;

/**
* @brief        ���ɱ��Ĵ���
* @param[in]     input_data: ��������
* @param[in]     input_len: �������ݳ���
* @param[out]    output_data: Ӧ���Ļ�����
* @param[in]     output_len: Ӧ���Ļ���������
* @return      �ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
*/
int32 (*acs_data_transfer)(struct tag_ACS_DEVICE *dev, uint8 *input_data, int32 input_len, uint8 *output_data, int32 output_len);

	// �û���չ����	
}ACS_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

