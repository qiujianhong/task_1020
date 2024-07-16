#include <hal.h>
#include <eeprom.h>
#include <string.h>

#define TEST_DATA_LEN  16
#define DATA_OFFSET    0
#define AT24C_WP_DISABLE    0               //write protect disalbe
#define AT24C_WP_ENABLE     1               //write protect enable

int main(void)
{
    int i = 0;
	int iRet = 0;
    uint8 recv_buf[TEST_DATA_LEN] = {0};
    uint8 send_buf[TEST_DATA_LEN] = {0};

    hal_init();
    EEPROM_DEVICE_T* dev = (EEPROM_DEVICE_T*)hal_device_get(HW_DEVICE_ID_EEPROM);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }

    memset(recv_buf, 0, sizeof(recv_buf));

	dev->eeprom_write_protect_enable(dev, AT24C_WP_DISABLE);

    iRet = dev->eeprom_read(dev, recv_buf, DATA_OFFSET, 16);
    printf("read eeprom offset(0-16) data: %d\n  ", iRet);
    for(i = 0; i< TEST_DATA_LEN; i++)
    {
        printf("0x%02x ", recv_buf[i]);
		send_buf[i] = recv_buf[i] + 0x02;
    }
	
	printf("\n");
    
    sleep(1);
    
    iRet = dev->eeprom_write(dev, send_buf, DATA_OFFSET, sizeof(send_buf));
    printf("\n write new data to eeprom  %d\n", iRet);
    sleep(1);
    memset(recv_buf, 0x0, sizeof(recv_buf));
    iRet = dev->eeprom_read(dev, recv_buf, DATA_OFFSET, sizeof(recv_buf));
    printf("read  offset(0-64) new data from eeprom: %d\n  ", iRet);
    for(i = 0; i< TEST_DATA_LEN; i++)
    {
        printf("0x%02x ", recv_buf[i]);
		send_buf[i] = recv_buf[i] - 0x02;
    }

	printf("\n");

    sleep(1);
	
    dev->eeprom_write(dev, send_buf, DATA_OFFSET, sizeof(send_buf));   
    printf("\n write data back to eeprom %d \n",iRet);
    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
