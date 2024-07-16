#include "types.h"
#include <hal.h>
#include <key.h>

int main(void)
{
    int cnt = 0;
    int ret = 0;
    KEY_VALUE_T akey[10];
    uint32 presstime;
	hal_init();
    KEY_DEVICE_T* dev = (KEY_DEVICE_T*)hal_device_get(HW_DEVICE_ID_KEY);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }
    
	while(cnt++ < 20)
    {
        ret = dev->key_value_get(dev, &akey[0], sizeof(KEY_VALUE_T));
        if(0 < ret)
        {
            printf("key %d\n",akey[0].key);
        }
        else
        {
            printf("no key press\n");
        }
        dev->key_press_time_get(dev,&presstime);
        
        printf(" press time %d\n",presstime);
        sleep(1);
	}
	
    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
