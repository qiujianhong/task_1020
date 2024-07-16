#include <hal.h>
#include "types.h"
#include <lcd.h>

int main(void)
{
    int cnt = 0;
    TLCD_PIXEL_T pixel;
	uint32 device_id = 0;
	uint8 buffer[3200] ={0};

	hal_init();
    LCD_DEVICE_T* dev = (LCD_DEVICE_T*)hal_device_get(HW_DEVICE_ID_LCD);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }

    memset(&pixel, 0, sizeof(pixel));
#if 0
    dev->lcd_info_get(dev, &pixel);
    printf("x=%d, y=%d\n", pixel.pixel_row, pixel.pixel_col);
    printf("LCD off\n");
    dev->lcd_bkl_set(dev, LCD_BKL_OFF);
    sleep(1);
    printf("LCD on\n");
    dev->lcd_bkl_set(dev, LCD_BKL_ON);
    dev->app_regist_id = 0;
    dev->lcd_active_id_set(dev, 0);
	if(dev->lcd_active_id_get(dev, &device_id) == 0)
		printf("get device id = %d\n", device_id);
		
   	while(cnt++ < 100)
    {
        uint8 uDispByte = 0xff<<(cnt%8);
		memset(buffer , uDispByte , 3200);
		dev->lcd_brush_region(dev, 10, 16, 150, 159, buffer, ((159 - 16+1)*(150 - 10+1)/8) );
        sleep(1);
	}
#endif
	
    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
