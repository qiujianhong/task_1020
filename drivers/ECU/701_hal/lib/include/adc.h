#ifndef _ADC_H
#define _ADC_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>


#define   HW_DEVICE_ID_ADC  "adc"
#define   ADC_LIB_PATH      "/lib/hal_lib/libhal_adc.so"    /*adc ��̬��·�� */

typedef enum
{
    EMADC_RTCVOL_CHANNEL = 0,  /*ʱ�ӵ��ADCͨ���� */
    EMADC_BATVOL_CHANNEL,      /*���õ��ADCͨ���� */
    EMADC_TEMP_CHANNEL,        /*�¶ȴ�����ADCͨ���� ��λ 0.1��*/
    EMADC_CAPVOL_CHANNEL,     /* �������ݵ�ѹ */
    EMADC_MAX_CHANNEL = 255,   /*ADCͨ��������޶�ֵ */
}ADC_CHANNEL_E;

/* ����ADCģ��ṹ */
typedef struct tag_ADC_MODULE 
{
    HW_MODULE common;
}ADC_MODULE;

/*����ADC�豸�ṹ */
typedef struct tag_ADC_DEVICE
{
	HW_DEVICE	base;
   /**
	* @brief ��ȡָ��ADCͨ������
	* @param[in] dev: �豸����
	* @param[in] index: ADCͨ����ţ�0~max��
	* @param[out] data: ADCͨ�����ݣ���λ��mv��
	* @note pData����ת��֮���ģ���ѹֵ�� ��λΪmv�� ����ֱ�ӷ���
	  ADC�����ļĴ���������
	* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
	*/
	int32 (*adc_volt_read)(struct tag_ADC_DEVICE *dev, ADC_CHANNEL_E index, int32 *data);
	// �û���չ����	

}ADC_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

