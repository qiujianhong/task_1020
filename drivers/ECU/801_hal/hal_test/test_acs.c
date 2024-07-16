#include "types.h"
#include <hal.h>
#include <acs.h>

int main(void)
{
    //uint8 input[30]={0};
    //uint8 output[30] = {0};
    hal_init();
    ACS_DEVICE_T* dev = (ACS_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ACS);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }
    
    {
       //// dev->acs_data_transfer(input, sizeof(input), output, sizeof(output) );
        sleep(1);
    }
	
    hal_device_release((HW_DEVICE *)dev);

    return 0;
} 
