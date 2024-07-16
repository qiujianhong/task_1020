#include "types.h"
#include <devinfo.h>
#include <dirent.h>
#include "sysDefine.h"

/* devinfo file 路径 */
#define   APP_ID_STORE_PATH      "/data/devinfo/" 

#define   APP_ID_BACKUP_PATH      "/backup/" 

#define DEVINFO_DEV_NUM          1
static char *aDevinfoDevice[DEVINFO_DEV_NUM] = {HW_DEVICE_ID_DEVINFO };


#define APPID_LEN                 24
#define PPPINITFCS16 0xffff                /* Initial FCS value */
#define PPPGOODFCS16 0xf0b8                /* Good final FCS value */

const uint16 hal_fcstab[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

#define APPLIC_LEN                 36

#define MTD_UBOOT_DEV   "/backup/uboot.img"
#define MTD_KERNEL_DEV   "/backup/boot.img"
#define IMG_SIZE_OFFSET  68    /* 64bytes +4 bytes */
#define SIGN_SIZE  64    /* 64bytes  */
/**
* @brief读取内核大小
* @return返回内核大小。
*/
int32 devinfo_get_kernel_size(struct tag_DEVINFO_DEVICE *dev)
{
    int32 iRet =0;
    int32 fdr = 0;
    int32 imglen = 0;
    uint8 uKernelLen[4] = {0};

    printf("devinfo_get_kernel_size\n");
    fdr = open(MTD_KERNEL_DEV, O_RDONLY);
    if(fdr < 0)
    {
        printf("open %s dev",MTD_KERNEL_DEV);
        return ERR_NORMAL;
    }

    imglen = lseek(fdr, 0, SEEK_END);
    if(imglen <= 0)
          return ERR_INVAL;

    printf(" kernel size imglen %d\n", imglen); 
    lseek (fdr, (imglen-IMG_SIZE_OFFSET), SEEK_SET);
    iRet = read(fdr, uKernelLen, sizeof(uKernelLen));
    if(iRet < 0)
    {
        perror("read");
        return ERR_READWR;
    }

    iRet = uKernelLen[0]<<24|uKernelLen[1]<<16|uKernelLen[2]<<8|uKernelLen[3];

    printf("kernel size %x\n", iRet);
    close(fdr);
    return iRet;

}
/**
* @brief 读取内核数据
* @param[out] read_buf: 读取内核数据的buffer
* @param[in] read_len: 读取一定大小的内核数据
* @param[in] read_kernel_dataoffset: 读取内核数据的起始偏移
* @return成功返回大于0的数据长度，失败返回错误码。
*/
int32 devinfo_read_kernel_data(struct tag_DEVINFO_DEVICE *dev,
        uint8 *read_buf, uint32 read_len, uint32 offset)
{
    int32 cnt = 0;
    int32 fdr = 0;
    int32 imglen = 0;
	  
    printf("devinfo_read_kernel_data\n");
    if((read_buf == NULL) || (read_len == 0))
        return ERR_PTR;
	  
    fdr = open(MTD_KERNEL_DEV, O_RDONLY);
    if(fdr == -1)
    {
        printf("open %s dev",MTD_KERNEL_DEV);
        return ERR_NORMAL ;
    }

    imglen = lseek(fdr, 0, SEEK_END);
    if(offset > imglen)
    	  return ERR_INVAL;
    	  
    lseek (fdr, offset, SEEK_SET);
    printf(" read_kernel_data %d \n", offset);
    cnt = read(fdr, read_buf, read_len);
    if(cnt < 0)
    {
        perror("read");
        return ERR_READWR;
    }
    
    printf(" read_kernel_data ret %d \n", cnt);
    close(fdr);

    return cnt;
}

/**
* @brief读取bootloader大小
* @return返回bootloader大小。
*/
int32 devinfo_get_bootloader_size(struct tag_DEVINFO_DEVICE *dev)
{
    int32 iRet =0;
    int32 fdr = 0;
    int32 imglen = 0;
    uint8 uUbootLen[4] = {0};

    printf("devinfo_get_bootloader_size\n");
    fdr = open(MTD_UBOOT_DEV, O_RDONLY);
    if(fdr < 0)
    {
        printf("open %s dev",MTD_UBOOT_DEV);
        return ERR_NORMAL;
    }

    imglen = lseek(fdr, 0, SEEK_END);
    if(imglen <= 0)
          return ERR_INVAL;

    printf(" uboot imglen %d\n", imglen); 
    lseek (fdr, (imglen-IMG_SIZE_OFFSET), SEEK_SET);
    iRet = read(fdr, uUbootLen, sizeof(uUbootLen));
    if(iRet < 0)
    {
        perror("read");
        return ERR_READWR;
    }

    iRet = uUbootLen[0]<<24|uUbootLen[1]<<16|uUbootLen[2]<<8|uUbootLen[3];

    printf("uboot size %d\n", iRet);
    close(fdr);
    return iRet;

}
/**
* @brief 读取bootloader数据
* @param[out] read_buf: 读取bootloader数据的buffer
* @param[in] read_len: 读取一定大小的bootloader数据
* @param[in] offset: 读取bootloader数据定的起始偏移
* @return成功返回大于0的数据长度，失败返回错误码。
*/
int32 devinfo_read_bootloader_data(struct tag_DEVINFO_DEVICE *dev,
              uint8 *read_buf, uint32 read_len, uint32 offset)
{
    int32 cnt = 0;
    int32 fdr = 0;
    int32 imglen = 0;
	  
    printf("devinfo_read_bootloader_data\n");
    if((read_buf == NULL) || (read_len == 0))
        return ERR_PTR;
	  
    fdr = open(MTD_UBOOT_DEV, O_RDONLY);
    if(fdr < 0)
    {
        printf("open %s dev",MTD_UBOOT_DEV);
        return ERR_NORMAL;
    }

    imglen = lseek(fdr, 0, SEEK_END);
    if(offset > imglen)
    	  return ERR_INVAL;
    	  
    printf(" read_bootloader_data\n");
    lseek (fdr, offset, SEEK_SET);
    cnt = read(fdr, read_buf, read_len);
    if(cnt < 0)
    {
        perror("read");
        return ERR_READWR;
    }

    close(fdr);
        
    return cnt;
}
/**
* @brief 读取BootLoader签名id
* @param[out] cert_id: 读取BootLoader签名id的缓冲区
cert_id = NULL,获取BootLoader签名id的长度
* @return成功返回大于0的BootLoader签名id的长度，失败返回错误码。
*/
int32 devinfo_read_bootloader_cert_id(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_id)
{
    int32 iRet =0;
    int32 fdr = 0;
    int32 imglen = 0;
    uint8 uUbootLen[4] = {0};
    int   iImgSize = 0;

    printf("devinfo_read_bootloader_cert_id\n");
    if(dev == NULL)
    {
        printf("devinfo_read_bootloader_cert_id: dev is null\n");
        return ERR_PTR;
    }

    fdr = open(MTD_UBOOT_DEV, O_RDONLY);
    if(fdr < 0)
    {
        printf("open %s dev",MTD_UBOOT_DEV);
        return ERR_NORMAL;
    }

    imglen = lseek(fdr, 0, SEEK_END);
    if(imglen <= 0)
          return ERR_INVAL;
 
    lseek (fdr, (imglen-IMG_SIZE_OFFSET), SEEK_SET);
    iRet = read(fdr, uUbootLen, sizeof(int32));
    if(iRet < 0)
    {
        perror("read");
        return ERR_READWR;
    }

    iImgSize = uUbootLen[0]<<24|uUbootLen[1]<<16|uUbootLen[2]<<8|uUbootLen[3];
    
    lseek (fdr, iImgSize, SEEK_SET);
    if(cert_id != NULL)
    {
        iRet = read(fdr, cert_id, (imglen-iImgSize-SIGN_SIZE));
        if(iRet < 0)
        {
            printf("uboot cert id read err %d\n", iRet);
            return ERR_READWR;
        }
    }
    else
        iRet =  (imglen-iImgSize-SIGN_SIZE);  
    
    printf("uboot cert_id len %d\n", iRet); 
    close(fdr);
    return iRet;
}
/**
* @brief 读取BootLoader签名
* @param[out] cert_buf: 读取签名数据的缓冲区
cert_buf = NULL,获取bootloader签名长度
* @return成功返回大于0的数据签名长度，失败返回错误码。
*/
int32 devinfo_read_bootloader_sign(struct tag_DEVINFO_DEVICE *dev, uint8 *cert_buf)
{
    int32 iRet =0;
    int32 fdr = 0;
    int32 imglen = 0;

    printf("devinfo_read_bootloader_sign\n");
    if(dev == NULL)
    {
        printf("devinfo_read_bootloader_sign: dev is null\n");
        return ERR_PTR;
    }

    fdr = open(MTD_UBOOT_DEV, O_RDONLY);
    if(fdr < 0)
    {
        printf("open %s dev",MTD_UBOOT_DEV);
        return ERR_NORMAL;
    }

    imglen = lseek(fdr, 0, SEEK_END);
    if(imglen <= 0)
          return ERR_INVAL;

    lseek (fdr, (imglen-SIGN_SIZE), SEEK_SET);
    if(cert_buf != NULL)
    {
         iRet = read(fdr, cert_buf, SIGN_SIZE);
         if(iRet < 0)
         {
             perror("read");
             return ERR_READWR;
         }
    }
    else 
         iRet = SIGN_SIZE;

    close(fdr);
    return iRet;
}
/**
* @brief 读取内核签名id
* @param[out] cert_id: 读取内核签名id的缓冲区
cert_id = NULL,获取内核签名id的长度
* @return成功返回大于0的内核签名id的长度，失败返回错误码。
*/
int32 devinfo_read_kernel_cert_id(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_id)
{
    int32 iRet =0;
    int32 fdr = 0;
    int32 imglen = 0;
    uint8 uKernelLen[4] = {0};
    int   iImgSize = 0;

   printf("devinfo_read_kernel_cert_id\n");
   if(dev == NULL)
   {
        printf("devinfo_read_kernel_cert_id: dev is null\n");
        return ERR_PTR;
   }

    fdr = open(MTD_KERNEL_DEV, O_RDONLY);
    if(fdr < 0)
    {
        printf("open %s dev",MTD_KERNEL_DEV);
        return ERR_NORMAL;
    }

    imglen = lseek(fdr, 0, SEEK_END);
    if(imglen <= 0)
          return ERR_INVAL;

    printf(" read cert_id kernel imglen %d\n", imglen); 
    lseek (fdr, (imglen-IMG_SIZE_OFFSET), SEEK_SET);
    iRet = read(fdr, uKernelLen, sizeof(int32));
    if(iRet < 0)
    {
        perror("read");
        return ERR_READWR;
    }

    iImgSize = uKernelLen[0]<<24|uKernelLen[1]<<16|uKernelLen[2]<<8|uKernelLen[3];
    
    lseek (fdr, iImgSize, SEEK_SET);
    if(cert_id != NULL)
    {
        iRet = read(fdr, cert_id, (imglen-iImgSize-SIGN_SIZE));
        if(iRet < 0)
        {
            printf("kernel cert id read err %d\n", iRet);;
            return ERR_READWR;
        }
    }
    else
        iRet = imglen-iImgSize-SIGN_SIZE;
    
    printf(" kernel certid len %d\n", iRet);
    close(fdr);
    return iRet;
}
/**
* @brief 读取内核签名
* @param[out] cert_buf: 读取内核签名的缓冲区
cert_buf = NULL,获取内核签名的长度
* @return成功返回大于0的内核签名的长度，失败返回错误码。
*/
int32 devinfo_read_kernel_sign(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_buf)
{
    int32 iRet =0;
    int32 fdr = 0;
    int32 imglen = 0;

    printf(" devinfo_read_kernel_sign\n");
    if(dev == NULL)
    {
        printf(" devinfo_read_kernel_sign: dev is null\n");
        return ERR_PTR;
    }

    fdr = open(MTD_KERNEL_DEV, O_RDONLY);
    if(fdr < 0)
    {
        printf("open %s dev",MTD_KERNEL_DEV);
        return ERR_NORMAL;
    }

    imglen = lseek(fdr, 0, SEEK_END);
    if(imglen <= 0)
          return ERR_INVAL;

    printf("read_kernel_sign\n");
    lseek (fdr, (imglen-SIGN_SIZE), SEEK_SET);
    if(cert_buf != NULL)
    {
        iRet = read(fdr, cert_buf, SIGN_SIZE);
        if(iRet < 0)
        {
            perror("read");
            return ERR_READWR;
        }
    }
    else
        iRet = SIGN_SIZE;

    printf("read_kernel_sign iRet %d\n",iRet );
    close(fdr);
    return iRet;
}


uint16 HAL_CRC16_GetEx(uint16 CRC16_data,const uint8* buf, uint32 len)
{
	const unsigned char *cp=buf;
	
	while (len--)
		CRC16_data = (CRC16_data >> 8) ^ hal_fcstab[(CRC16_data ^ *cp++) & 0xff];
		
	return (CRC16_data);
}

uint16 HAL_CRC16_Get(const uint8* buf, uint32 len)
{
	uint16 trialfcs;
	
	/* add on output */
	trialfcs = HAL_CRC16_GetEx( PPPINITFCS16, buf, len );
	trialfcs ^= 0xffff; /* complement */
	
	return trialfcs;
}


int32  WriteIDFile( char *devinfofile, const char *appid, int32 appid_size)
{
	
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;
    uint16  uCrcCode = 0;

    if((devinfofile == NULL) || (appid == NULL))
   	    return ERR_PTR;

    fd = open(devinfofile, O_CREAT|O_WRONLY,0666);
    if(fd < 0)
    {
	      DEBUG("open %s error!\n", devinfofile);
              return ERR_NORMAL;   
    }

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
   
    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
        printf("Get id Return value of fcntl:%d\n",iRet);

    iRet = write(fd, appid, appid_size);
    if(iRet != appid_size)
    {
	      printf("active_id write error\n");
		    iRet = ERR_NORMAL;
    }
    else 
    {
 	    iRet = ERR_OK;
    }
	  
    uCrcCode = HAL_CRC16_Get((const uint8*)appid, appid_size);
	  
    iRet = write(fd, &uCrcCode, sizeof(uCrcCode)); 
    if(iRet != sizeof(uCrcCode))
    {
            printf("appid write crc error \n");
		    iRet = ERR_NORMAL;
    }
    else 
    {
 	    iRet = ERR_OK;
    }
	  
    lock.l_type = F_UNLCK;
    iRet = fcntl(fd, F_SETLK, &lock);
    if (iRet < 0)
    {
	      printf("fcntl failed msg:%s\n", strerror(errno));
	      iRet =  ERR_LOCK;
    }
   
    close(fd);

    return iRet;
}

int32 WriteLicFile(char *devinfofile, const char *appname, const char *license, int32 license_size)
{
    int32 fd = 0;
    int32 iRet= 0;
    uint8 namelen = 0;
    
    struct flock lock;

    if(NULL == devinfofile || NULL == appname || NULL == license)
    {
        return ERR_PTR;
    }
    
    fd = open(devinfofile, O_CREAT|O_WRONLY,0666);
    if(fd < 0)
    {
        DEBUG("open %s error!\n", devinfofile);
        return ERR_NORMAL;   
    }

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
    {
        printf("Get id Return value of fcntl:%d\n",iRet);
    }
    iRet = write(fd, license, license_size);
    if(iRet != license_size)
    {
        printf("active_id write error\n");
        iRet = ERR_NORMAL;
    }
    else 
    {
        iRet = ERR_OK;
    }
    namelen = strlen(appname);
    write(fd, &namelen, 1);
    write(fd, appname, namelen);

    lock.l_type = F_UNLCK;
    iRet = fcntl(fd, F_SETLK, &lock);
    if (iRet < 0)
    {
        printf("fcntl failed msg:%s\n", strerror(errno));
        iRet =  ERR_LOCK;
    }

    close(fd);
    return iRet;
}



/**
* @brief        读取appid
* @param[in]     appname: app英文名称
* @param[out]    appid: 对应app的ID信息
* @param[in]     max_appid_size: 存放appid的缓冲区大小
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32 devinfo_read_appid(struct tag_DEVINFO_DEVICE *dev, const char *appname, char *appid, int32 max_appid_size)
{
    int32 fd = 0;
   int32 iRet= 0;
   struct flock lock;
   char devinfofile[256] ={0};
   uint16  uCrcCode = 0;
   uint16  rd_uCrcCode = 0;

   if((appname == NULL) || (appid == NULL))
   	   return ERR_PTR;

    sprintf(devinfofile, "%s%s", APP_ID_STORE_PATH, appname);
    if(0 != access(devinfofile, F_OK))
    {
        memset(devinfofile, 0, 256);
        sprintf(devinfofile, "%s%s", APP_ID_BACKUP_PATH, appname);
    }
    fd = open(devinfofile, O_CREAT|O_RDONLY,0666);
    if(fd < 0)
    {
       DEBUG("open %s error!\n", devinfofile);
       return ERR_NORMAL;   
    }
   
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
   
    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
        printf("Get id Return value of fcntl:%d\n",iRet);
   
    iRet = read(fd, appid, APPID_LEN ); 
    if(iRet < 0)
    {
       printf("active_id read error\n");
       iRet = ERR_NORMAL;
    }
    else
    {
        iRet = ERR_OK;
    }

    uCrcCode = HAL_CRC16_Get((const uint8*)appid, APPID_LEN );
   	iRet = read(fd, &rd_uCrcCode, sizeof(rd_uCrcCode)); 
	  if(iRet != sizeof(rd_uCrcCode))
    {
	      printf("appid read crc error \n");
		    iRet = ERR_NORMAL;
	  }
	  else 
	  {
	 	    iRet = ERR_OK;
	  }
	  
   lock.l_type = F_UNLCK;
   iRet = fcntl(fd, F_SETLK, &lock);
   if (iRet < 0)
   {
	     printf("fcntl failed msg:%s\n", strerror(errno));
	     iRet =  ERR_LOCK;
   }
   
   close(fd);
   
   if(uCrcCode == rd_uCrcCode)
   	   iRet = ERR_OK;
   else
   	   iRet = ERR_NORMAL;

   return iRet;
}


/**
* @brief        写appid
* @param[in]     appname: app英文名称
* @param[in]   appid: 对应app的ID信息
* @param[in]    appid_size: appid的大小
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32 devinfo_write_appid(struct tag_DEVINFO_DEVICE *dev, const char *appname, const char *appid, int32 appid_size)
{
    int32 iRet= 0;
    char  devinfofile[256] ={0};
    int32 status =  -1;

    if(appname == NULL)
   	    return ERR_PTR;

    if(access(APP_ID_STORE_PATH, F_OK) != 0)
    {
    	  status = mkdir(APP_ID_STORE_PATH, 0766);
    	  if(status < 0)
    	  {
    	  	  perror("mkdir /data/devinfo  error");   
            return ERR_NORMAL;  
    	  }
    }

    sprintf(devinfofile, "%s%s", APP_ID_STORE_PATH, appname);
    iRet = WriteIDFile(devinfofile, appid,  appid_size);
    
    memset(devinfofile, 0x0, sizeof(devinfofile));
    if(access(APP_ID_BACKUP_PATH, F_OK) != 0)
    {
    	  status = mkdir(APP_ID_BACKUP_PATH, 0766);
    	  if(status < 0)
    	  {
    	  	  perror("mkdir /backup  error");   
            return ERR_NORMAL;  
    	  }
    }
        	
    sprintf(devinfofile, "%s%s", APP_ID_BACKUP_PATH, appname);
    iRet = WriteIDFile(devinfofile, appid,  appid_size);

	 return iRet;
}

/**
* @brief 读取 APP 许可信息
* @param[in] appno: app 编号
* @param[out] appname: 对应 app 的名称
* @param[out] license: 对应 app 的许可信息
* @param[in] max_license_size: 存放 license 的缓冲区大小
* @return 成功返回 ERR_OK；失败返回错误码。
*/
int32 devinfo_read_applicense(struct tag_DEVINFO_DEVICE *dev, const char *appno, char *appname, char *license, 
                              int32 max_license_size)
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;
    char   devinfofile[256] ={0};
    char   licdata[APPLIC_LEN] = {0};
    char   noBig[256] = {0};
    int    offset = 0;
    int    len = 0;
    uint8  namelen = 0;

    if(appno == NULL || license == NULL)
    {
        return ERR_PTR;
    }

    /* app 编号全部替换成大写字母 */
    len = strlen(appno);
    if(0 == len)
    {
        return ERR_FORMAT;
    }
    for(offset = 0; offset < len; offset++)
    {
        if(('0' <= appno[offset] && appno[offset] <= '9') || ('A' <= appno[offset] && appno[offset] <= 'F'))
        {
            noBig[offset] = appno[offset];
        }
        else if('a' <= appno[offset] && appno[offset] <= 'f')
        {
            noBig[offset] = 'A' + (appno[offset] - 'a');
        }
        else
        {
            return ERR_FORMAT;
        }
    }

    sprintf(devinfofile, "%s%s.lic", APP_ID_STORE_PATH, noBig);
    fd = open(devinfofile, O_CREAT|O_RDONLY,0666);
    if(fd <= 0)
    {
        sprintf(devinfofile, "%s%s.lic", APP_ID_BACKUP_PATH, noBig);
        fd = open(devinfofile, O_CREAT|O_RDONLY,0666);
        if(fd <= 0)
        {
            return ERR_ACCESS;
        }
    }

    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
    printf("Get id Return value of fcntl:%d\n",iRet);

    iRet = read(fd, licdata, APPLIC_LEN); 
    if(iRet <= 0)
    {
        printf("active_id read error\n");
        iRet = ERR_NORMAL;
    }
    else
    {
        memcpy(license, licdata, (max_license_size >= iRet) ? iRet : max_license_size);
        lseek(fd, APPLIC_LEN, SEEK_SET);

        iRet = read(fd, &namelen, 1);
        if(iRet <= 0)
        {
            namelen = 0;
        }
        if(namelen > 0)
        {
            lseek(fd, APPLIC_LEN + 1, SEEK_SET);
            read(fd, appname, namelen); 
        }
        else
        {
            appname[0] = 0;
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return iRet;
}

/**
* @brief 写 app 许可信息
* @param[in] appno: app 编号
* @param[in] appname: 对应 app 的名称
* @param[in] license: 对应 app 的许可信息
* @param[in] license_size: app 许可信息的大小
* @return 成功返回 ERR_OK；失败返回错误码。
*/
int32 devinfo_write_applicense(struct tag_DEVINFO_DEVICE *dev, const char *appno, const char *appname, 
                               const char *license, int32 license_size)
{
    int32 iRet= 0;
    char  devinfofile[256] ={0};
    char  noBig[256] = {0};
    int   offset = 0;
    int   len = 0;
    int32 status =  -1;

    if(appno == NULL)
    {
        return ERR_PTR;
    }
    if(access(APP_ID_STORE_PATH, F_OK) != 0)
    {
        status = mkdir(APP_ID_STORE_PATH, 0766);
        if(status < 0)
        {
            perror("mkdir /data/devinfo  error");
            return ERR_NORMAL;  
        }
    }

    /* app 编号全部替换成大写字母 */
    len = strlen(appno);
    if(0 == len)
    {
        return ERR_FORMAT;
    }
    for(offset = 0; offset < len; offset++)
    {
        if(('0' <= appno[offset] && appno[offset] <= '9') || ('A' <= appno[offset] && appno[offset] <= 'F'))
        {
            noBig[offset] = appno[offset];
        }
        else if('a' <= appno[offset] && appno[offset] <= 'f')
        {
            noBig[offset] = 'A' + (appno[offset] - 'a');
        }
        else
        {
            return ERR_FORMAT;
        }
    }

    sprintf(devinfofile, "%s%s.lic", APP_ID_STORE_PATH, noBig);
    iRet = WriteLicFile(devinfofile, appname, license, license_size);
    sprintf(devinfofile, "%s%s.lic", APP_ID_BACKUP_PATH, noBig);
    WriteLicFile(devinfofile, appname, license, license_size);

    return iRet;
}

/** 
* @brief 当前 app 许可数量 
* @return 返回当前app许可数量
*/ 
uint32 devinfo_applicense_count( struct tag_DEVINFO_DEVICE *dev)
{
    char cmdcp[256] = {0};
    uint32 count = 0;
    DIR *dir = NULL;
    struct dirent *ptr = NULL;

    sprintf(cmdcp, "cp -r %s/*.lic %s/", APP_ID_BACKUP_PATH, APP_ID_STORE_PATH);
    system(cmdcp);
    sleep(1);
    system("sync");
    sleep(1);

    dir = opendir(APP_ID_STORE_PATH);
    if(NULL == dir)
    {
        return 0;
    }

    while(NULL != (ptr = readdir(dir)))
    {
        if(0 == strcmp(ptr->d_name, ".") || 0 == strcmp(ptr->d_name, ".."))
        {
            continue;
        }
        else if(DT_REG == ptr->d_type) //普通文件
        {
            if(NULL != strstr(ptr->d_name, ".lic"))
            {
                count++;
            }
        }
    }

    closedir(dir);
    return count;
}

/** 
* @brief 按序号读取 APP 许可信息 
* @param[in] index: 序号（从0开始） 
* @param[out] appno: 对应 app 的编号 
* @param[out] appname: 对应 app 的名称
* @param[out] license: 对应 app 的许可信息 
* @param[in] max_license_size: 存放 license 的缓冲区大小 
* @return 成功返回 ERR_OK；失败返回错误码。 
*/
int32 devinfo_read_applicense_index(struct tag_DEVINFO_DEVICE *dev, uint32 index, char *appno, char *appname, 
                                    char *license, int32 max_license_size)
{
    uint32 count = 0;
    int32  ret = ERR_NOTEXIST;
    DIR   *dir = NULL;
    struct dirent *ptr = NULL;

    dir = opendir(APP_ID_STORE_PATH);
    if(NULL == dir)
    {
        return ret;
    }

    while(NULL != (ptr = readdir(dir)))
    {
        if(0 == strcmp(ptr->d_name, ".") || 0 == strcmp(ptr->d_name, ".."))
        {
            continue;
        }
        else if(DT_REG == ptr->d_type) //普通文件
        {
            if(NULL != strstr(ptr->d_name, ".lic"))
            {
                count++;
                if(count == index + 1)
                {
                    memcpy(appno, ptr->d_name, strlen(ptr->d_name) - 4);
                    ret = devinfo_read_applicense(dev, appno, appname, license, max_license_size);
                    break;
                }
            }
        }
    }

    closedir(dir);
    return ret;
}


/**
* @brief 获取终端设备类型
* @return 成功返回 大于0的类型值。
          0x1NXXXXXX表示集中器类终端（例如：0x11XXXXXX表示I型集中器、0x12XXXXXX表示II型集中器）
          0x2NXXXXXX表示专变类终端（例如：0x21XXXXXX表示I型专变，0x22XXXXXX表示II型专变，0x23XXXXXX表示III型专变）
          0x3NXXXXXX表示能源控制器类终端（例如：0x31XXXXXX表示能源控制器专变）
          其中XXXXXX为厂家自定义，失败返回错误码
**/
int32 devinfo_get_device_type(struct tag_DEVINFO_DEVICE *dev)
{
    return 0x21FFFFFF;
}

/**
* @brief 获取终端CPU温度
* @return成功返回温度值，单位0.1℃；失败返回错误码。
*/
int32 devinfo_get_cpu_temperature(struct tag_DEVINFO_DEVICE *dev)
{
    char tempstr[40] = {0};
    int  tempvalue = 0;
    
    FILE* pipe = fopen("/sys/class/thermal/thermal_zone0/temp", "r");  
    if (!pipe)  
    {
        pipe = fopen("/sys/class/thermal/thermal_zone1/temp", "r");
    }
    if (!pipe)
    {
        return 0;
    }
    
    memset(tempstr, 0, 40);
    if(fgets(tempstr, 40, pipe) == NULL)
    {
        fclose(pipe);
        return 0;
    }

    tempvalue = atoi(tempstr);
    tempvalue = tempvalue / 100;
    fclose(pipe);
    return tempvalue;
}


static int devinfo_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*)malloc(sizeof(DEVINFO_DEVICE_T));
    if(dev == NULL)
    {
        printf("devinfo malloc failed \n");
        return -1;
    }

    memset(dev, 0, sizeof( DEVINFO_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_DEVINFO;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->devinfo_read_appid  = devinfo_read_appid;
    dev->devinfo_write_appid = devinfo_write_appid;
    dev->devinfo_read_applicense = devinfo_read_applicense;
    dev->devinfo_write_applicense = devinfo_write_applicense;
    dev->devinfo_applicense_count = devinfo_applicense_count;
    dev->devinfo_read_applicense_index = devinfo_read_applicense_index;

    dev->devinfo_get_kernel_size = devinfo_get_kernel_size;
    dev->devinfo_read_kernel_data = devinfo_read_kernel_data;
    dev->devinfo_get_bootloader_size = devinfo_get_bootloader_size;
    dev->devinfo_read_bootloader_data = devinfo_read_bootloader_data;
    dev->devinfo_read_bootloader_cert_id = devinfo_read_bootloader_cert_id;
    dev->devinfo_read_bootloader_sign = devinfo_read_bootloader_sign;
    dev->devinfo_read_kernel_cert_id = devinfo_read_kernel_cert_id;
    dev->devinfo_read_kernel_sign = devinfo_read_kernel_sign;
    dev->devinfo_get_device_type = devinfo_get_device_type;
    dev->devinfo_get_cpu_temperature = devinfo_get_cpu_temperature;
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int devinfo_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    DEVINFO_DEVICE_T *dev = (DEVINFO_DEVICE_T *)pDevice;
    if(dev == NULL)
    {
        printf("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
DEVINFO_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "devinfo",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-07-18",
        .nDevNum = DEVINFO_DEV_NUM,
        .szDevices = aDevinfoDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = devinfo_open,
        .pfClose = devinfo_close,
    },
};
