#include <hal.h>
#include <adc.h>

int main(void)
{
    int cnt = 0;
    uint32 data;

	hal_init();
    ADC_DEVICE_T* dev = (ADC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ADC);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }
    
	while(cnt++ < 50)
    {
        dev->adc_volt_read(dev, EMADC_RTCVOL_CHANNEL, &data);
        printf("adc value: %d\n", data);
        sleep(2);
	}
	
    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
