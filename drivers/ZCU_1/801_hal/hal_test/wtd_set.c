#include <hal.h>
#include <watchdog.h>

int main(int argc, char** argv)
{
    int timeout = 0;

    if(2 != argc)
    {
        return 0;
    }
    timeout = atoi(argv[1]);
    if(timeout <= 0)
    {
        timeout = 10;
    }
    else if(timeout >= 600)
    {
        timeout = 600;
    }
    
	hal_init();
    WATCHDOG_DEVICE_T* dev = (WATCHDOG_DEVICE_T*)hal_device_get(HW_DEVICE_ID_WATCHDOG);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }

    printf("set watchdog tome out %d\n", timeout);
    dev->watchdog_period_set(dev, timeout);
    sleep(1);
    dev->watchdog_clear(dev);
    hal_device_release((HW_DEVICE *)dev);

    return 0;
} 
