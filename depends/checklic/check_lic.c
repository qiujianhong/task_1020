#include <hal.h>
#ifndef uint8
#define uint8    unsigned char
#endif

#ifndef int8
#define int8    signed char
#endif

#ifndef uint16
#define uint16    unsigned short
#endif

#ifndef int16
#define int16    short
#endif

#ifndef uint32
#define uint32    unsigned int
#endif

#ifndef int32
#define int32    int
#endif
#include <devinfo.h>

void write_licnum(uint8 num)
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;

    fd = open("/tmp/licnum", O_CREAT|O_WRONLY,0666);
    if(fd < 0)
    {
        return;
    }
    
    iRet = write(fd, &statsu, 1);
    if(iRet != 1)
    {
        RSP_FMT_DEBUG("写入状态失败\n");
    }

    lock.l_type = F_UNLCK;
    iRet = fcntl(fd, F_SETLK, &lock);
    if (iRet < 0)
    {
        RSP_FMT_DEBUG("fcntl failed msg:%s\n", strerror(errno));
    }

    close(fd);
}

int main(void)
{
    unsigned int iRet = 0;

    hal_init();
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*)hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        printf("0\n");
        return -1;
    }

    iRet = dev->devinfo_applicense_count(dev);
    
	system("");
    printf("%d\n", iRet);
	return 0;
} 
