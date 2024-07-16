#include <hal.h>
#include "types.h"
#include <led.h>

int main(void)
{
    int cnt = 10;

	hal_init();
    LED_DEVICE_T* dev = (LED_DEVICE_T*)hal_device_get(HW_DEVICE_ID_LED);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }

    printf("LED test start\n");
	while(cnt--)
	{
      /*ÔËÐÐ */
      dev->led_state_set(dev, EMLED_RUN, LED_OFF, COM_MODE, NULL, 0 );
      sleep(1);
      dev->led_state_set(dev, EMLED_RUN, LED_ON, COM_MODE, NULL, 0);
      sleep(1);
  	
      /*ALARM */
      dev->led_state_set(dev, EMLED_ALARM, LED_OFF, COM_MODE, NULL, 0);
      sleep(1);
      dev->led_state_set(dev, EMLED_ALARM, LED_ON, COM_MODE, NULL, 0);
      sleep(1);
	}
   
    
    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
