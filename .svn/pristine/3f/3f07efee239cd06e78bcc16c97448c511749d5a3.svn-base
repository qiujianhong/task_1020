#include <hal.h>
#include "types.h"
#include <rtc.h>

int main(void)
{
    int iRet = 0;
    struct tm rtc;
    struct tm rtc1;

    hal_init();
    RTC_DEVICE_T* dev = (RTC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_RTC);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }
    memset(&rtc1, 0, sizeof(rtc1));

    iRet = dev->rtc_time_get(dev, &rtc1);
    
    if(iRet >= 0)
        printf("front read rtc time: %d-%d-%d %d:%d:%d\n", rtc1.tm_year, rtc1.tm_mon, rtc1.tm_mday, rtc1.tm_hour, rtc1.tm_min, rtc1.tm_sec);
    else
        printf("front read rtc get time is failed\n");

#if 0
    //printf("set 2020-2-28 23:59:50\n");
    memset(&rtc, 0, sizeof(rtc));
    rtc.tm_year = (2020 - 1900);
    rtc.tm_mon = 01;
    rtc.tm_mday = 28;
    rtc.tm_hour = 23;
    rtc.tm_min = 59;
    rtc.tm_sec = 50;
    
    iRet = dev->rtc_time_set(dev, &rtc);
    if(iRet < 0)
		printf("rtc set time is failed\n");
	
    sleep(1);
    memset(&rtc1, 0, sizeof(rtc1));
    iRet = dev->rtc_time_get(dev, &rtc1);
    
	if(iRet >= 0)
        printf("after rtc time: %d-%d-%d %d:%d:%d\n", rtc1.tm_year, rtc1.tm_mon, rtc1.tm_mday, rtc1.tm_hour, rtc1.tm_min, rtc1.tm_sec);
    else
        printf("after rtc get time is failed\n");
#endif
    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
