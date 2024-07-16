#include <hal.h>
#include "types.h"
#include "sysDefine.h"

#include <uart.h>



int recv_data(UART_DEVICE_T* dev, uint32 baud, uint8 *buf, int len)
{
    int fd = 0;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int count = 0;
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;

    if(dev == NULL)
    {
        //DEBUG("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return ERR_PTR;
    }
 
    fd = dev->uart_get_handle(dev);
    
    DEBUG("1 uart_recv_data dev->fd = %d %s\n", dev->fd, dev->base.szDeviceID);
    
    if (0 == fd)  
    {  
        DEBUG("Can't Open Serial Port\n");  
        return ERR_NORMAL;  
    }

    recv_len = 0;                               
    data_len = 0;

 /*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
  一个字节包括起始位、停止位、校验位、8位数据 共11位*/
    if ((baud%300 != 0 ) || baud == 0)             // 若波特率不对，默认9600
    {   
        delay = 11000000/9600;
    }
    else
    {   
        delay = 11000000/baud;
    }

    while (1)
    {
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeout.tv_sec = 0;
        if (baud < 19200)
        {
            timeout.tv_usec = 30000;
        }
        else
        {
            timeout.tv_usec = 1000;
        }
        
        do {
            n = select(fd+1, &set, NULL, NULL, &timeout);
        }while(n<0);
  
        if (n > 0)  
        {
            //recv_len = read(fd, buf+data_len, len-data_len);
            recv_len = dev->uart_data_recv(dev, buf+data_len, len-data_len);
        }
        else
        {
            recv_len = 0;
        }

        // 收到字节后延时一个字节的时间再去接收
        if (recv_len > 0)   
        {
            data_len += recv_len;
            count = 0;
            usleep(delay);
        }
        else
        {
            if ((data_len == 0) && (count > 5)) 
            {
                usleep(delay);
                return data_len;
            }
            // 连续5次收不到字节认为无数据传输
            if ((data_len != 0) && ((count > 5) || (data_len > len))) 
            {
                usleep(delay);
                return data_len;                    
            }
        }
  
        count++;
    }

    return data_len;
}



int main(void)
{
    uint8 buf[512] = {0};
    //uint8 bufsnd[] = {0x68,0x03,0x00,0x42,0x01,0x02,0x00,0x00,0xB7,0x5A,0x16};
    //uint8 bufsnd[] = {0x68,0x03,0x00,0x42,0x02,0x02,0x00,0x01,0xF3,0x6E,0x16};
    uint8 bufsnd[] = {
        0x68, 0x17, 0x00, 0x43, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x48, 0x5E, 0x05, 0x01, 0x00, 0xF1, 0x01, 0x02, 0x00, 0x00, 0x2D, 0x56, 0x16
    };
    char devname[48] = {0};
    int ret = 0;
    
    int recv_len = 0;
    int snd_len = 0;
    int i = 0;
    int m = 0;
    uint8 status = 0;
    // 初始化HAL接口
	hal_init();
    UART_DEVICE_T* dev = (UART_DEVICE_T*)hal_device_get(UART_1_1_DEVICE_ID_STR);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }
    dev->uart_get_real_dev_name(dev, devname);
    printf("uart dev %s\n", devname);
    
    while (m++ < 20)
    {
        switch (status)
        {
            case 0:
                // 打开设备
                ret = dev->uart_param_set(dev, 115200, 8, 1, PARITY_EVEN);
                printf("set param ret %d\n", ret);
                status = 1;
                break;
            
            case 1:
                // 发送数据
                snd_len = dev->uart_data_send(dev, bufsnd, sizeof(bufsnd));
                printf("snd_len = %d\n", snd_len);
                if (snd_len > 0)
                {
                    for (i = 0; i < snd_len; i++)
                    {
                        printf("%02x ", bufsnd[i]);
                    }
                    printf("\n");
                }            
                status = 2;
                break;
            
            case 2:
                // 接收数据
                recv_len = recv_data(dev, 2400, buf, sizeof(buf));
                //recv_len = dev->uart_data_recv(dev, buf, sizeof(buf));            
                printf(" recv_len = %d\n", recv_len);
                if (recv_len > 0)
                {
                    for (i = 0; i < recv_len; i++)
                    {
                        printf("%02x ", buf[i]);
                    }
                    printf("\n");
                }
                status = 1;
                break;

            case 3:    
                // 关闭设备
                hal_device_release((HW_DEVICE *)dev);
                break;
        }
        sleep(1);
    }
    
    hal_device_release((HW_DEVICE *)dev);
    return 0;
} 
