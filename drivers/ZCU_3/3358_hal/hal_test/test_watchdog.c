#include <hal.h>
#include <watchdog.h>

int main(void)
{
    int cnt = 0;

	hal_init();
    WATCHDOG_DEVICE_T* dev = (WATCHDOG_DEVICE_T*)hal_device_get(HW_DEVICE_ID_WATCHDOG);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }

    printf("set watchdog period\n");
    dev->watchdog_period_set(dev, 10);
	while(cnt++ < 10)
    {
        printf("feed watch\n");
        dev->watchdog_clear(dev);
        sleep(1);
	}
	
    printf("watchdog reset\n");
    dev->watchdog_rst(dev);
    
    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
