#include "types.h"
#include <devinfo.h>
#include <dirent.h>
#include "sysDefine.h"

/* devinfo file 路径 */
#define   APP_ID_STORE_PATH      "/data/devinfo/" 
#define   APP_ID_BACKUP_PATH     "/backup/"
#define   APP_TEMP_LIC_PATH      "/tmp/lic_files/"


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
#define MTD_NEW_UBOOT_DEV   "/tmp/uboot.img"
#define MTD_NEW_KERNEL_DEV   "/tmp/boot.img"

#define IMG_SIZE_OFFSET  68    /* 64bytes +4 bytes */
#define SIGN_SIZE  64    /* 64bytes  */

#define IH_NMLEN 32
typedef struct image_header {
	unsigned int		ih_magic;	/* Image Header Magic Number	*/
	unsigned int		ih_hcrc;	/* Image Header CRC Checksum	*/
	unsigned int		ih_time;	/* Image Creation Timestamp	*/
	unsigned int		ih_size;	/* Image Data Size		*/
	unsigned int		ih_load;	/* Data	 Load  Address		*/
	unsigned int		ih_ep;		/* Entry Point Address		*/
	unsigned int		ih_dcrc;	/* Image Data CRC Checksum	*/
	unsigned char		ih_os;		/* Operating System		*/
	unsigned char		ih_arch;	/* CPU architecture		*/
	unsigned char		ih_type;	/* Image Type			*/
	unsigned char		ih_comp;	/* Compression Type		*/
	unsigned char		ih_name[IH_NMLEN];	/* Image Name		*/
} image_header_t;

typedef struct tagHeaderInfo{
	unsigned int  magic;
	unsigned int  header_size;
	unsigned int  header_ver;           // 固定为1
	unsigned int  content_len;
	unsigned char content_md5[32];
	unsigned int  reserved[19];
	unsigned short res16;
	unsigned short header_crc;
}HeaderInfo_t;

#define  uboot_dev_name  "/dev/mmcblk0p1"
//#define  uboot_dev_name  "/dev/mmcblk0boot0"
#define  kernel_dev_name "/dev/mmcblk0p5"

image_header_t kernel_header;
HeaderInfo_t HeaderInfo;


#if 1

/* 后4K长度给许可 */
/* 前64字节存储数量: 1字节校验+1字节数量+62字节备份 */
/* 后续按照96字节一组作为许可存储字节: 1字节校验+1字节名称长度+名称+24字节编号+36字节许可 */


/*eeprom 设备路径 */
#define EEPROM_DEV_FILE      "/sys/bus/i2c/devices/2-0050/eeprom"   /*eeprom映射成的文件 */

/**********************************************************
* @brief 写数据
* @param[in] pBuffer: 数据缓存
* @param[in] nOffest: 起始偏移地址
* @param[in] nLength: 写入数据长度
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
***********************************************************/
static int32 eeprom_write(uint8* pBuffer, uint32 nOffest, uint32 nLength)
{
    int32 iRet = 0;
    int32 fd = 0;
    int32 i,n;
    int32 seg_cnt;    //按512字节为一段，要写的数据分为seg_cnt段
    int32 remainder;  //分段后剩余的字节数
    
    //写缓存不能为空
    if (pBuffer == NULL) {
        DEBUG("Write buffer is NULL.\n");
        return ERR_PTR;
    }
    //写数据的范围是8K以内
    if (nOffest + nLength < 1 || nOffest + nLength > 8192) {
        DEBUG("Write eeprom out of range(should be in 8K).\n");
        return ERR_NORMAL;
    }

    fd = open(EEPROM_DEV_FILE, O_RDWR, 0666);
    if(fd == -1) 
    {
        DEBUG("eeprom dev open error\n");
        return ERR_NORMAL;
    }
    //设置写锁
    if( flock(fd, LOCK_EX) ){
       DEBUG("Lock Error");
	   iRet = ERR_LOCK;
        goto errexit;
    }

    usleep(10000);		

    seg_cnt = nLength / 512;
    remainder = nLength % 512;

    //每512字节一写
    for (i = 0; i < seg_cnt; i++) 
    {
        //定位
        if (lseek(fd, nOffest+(i*512), SEEK_SET) < 0) {
            DEBUG("eeprom locate error.\n");
	     iRet = ERR_IO;
            goto errexit;
        }
        //写入
        n = write(fd, pBuffer+(i*512), 512);
        if (n < 0) {
            DEBUG("eeprom write error.\n");
	     iRet = ERR_IO;
            goto errexit;
        } 
        else if (n<512 && n>=0){
            DEBUG("In segment %d write eeprom less then expectation.\n",i);
            break;
        }
        iRet += n;	//记录实际读到的字节数作为函数返回值
    }

    usleep(10000);	
    //写分段后的剩余部分 
    if ( (i == seg_cnt) ) 
    {
        //定位
        if (lseek(fd, nOffest+(seg_cnt*512), SEEK_SET) < 0) {
            DEBUG("eeprom locate error.\n");
	     iRet = ERR_IO;
            goto errexit;
        }
        //写入
        n = write(fd, pBuffer+(seg_cnt*512), remainder);
        if (n  < 0) {
            DEBUG("eeprom write error.\n");
	     iRet = ERR_IO;
            goto errexit;
        } 
        else if (n<remainder && n>=0){
            DEBUG("In remainder write eeprom less then expectation.\n");
        }
		
        iRet += n;
    }

    flock(fd, LOCK_UN);
    close(fd);
	iRet = (iRet >= 0)?ERR_OK : ERR_NORMAL;
    return iRet;

errexit:
    flock(fd, LOCK_UN);
    close(fd);
    return  iRet;
}

/**********************************************************
* @brief 读数据
* @param[out] pBuffer: 缓冲区指针
* @param[in] nOffest: 起始偏移地址
* @param[in] nLength: 缓冲区长度
* @return 成功返回读出的大于0的数据长度；失败返回错误码
***********************************************************/
static int eeprom_read(uint8* pBuffer, uint32 nOffest, uint32 nLength)
{
    int32 iRet = ERR_OK;
    int32 fd = 0;

	//读缓存不能为空
	if (pBuffer == NULL) 
    {
        printf("Eeprom read buffer is NULL.\n");
        return ERR_PTR;
    }
	
	//读取的数据要在8K以内
	if (nOffest + nLength < 1 || nOffest + nLength > 8192) 
    {
		printf("Read eeprom out of range.\n");
		return ERR_NORMAL;
	}

    fd = open(EEPROM_DEV_FILE, O_RDWR, 0666);
    if(fd == -1) 
    {
        printf("eeprom dev open error\n");
        return ERR_NORMAL;
    }
	
    if(flock(fd, LOCK_EX))
    {
        printf("eeprom lock Error");
		iRet = ERR_LOCK;
        goto errexit;
    }
    
	usleep(10000);	

	int i,n;
	int seg_cnt;	//按512字节为一段，要读的数据分为seg_cnt段
	int remainder;	//分段后剩余的字节数
	seg_cnt = nLength / 512;
	remainder = nLength % 512;

	//每512字节一读取
	for (i=0; i<seg_cnt; i++) 
	{
		//定位
		if (lseek(fd, nOffest+(i*512), SEEK_SET) < 0) {
			printf("eeprom locate error.\n");
			goto errexit;
		}
		//读取
		n = read(fd, pBuffer+(i*512), 512);
		if (n < 0) 
        {
			printf("eeprom read error.\n");
			goto errexit;
		} 
        else if (n<512 && n>=0)
        {
			printf("In segment %d read eeprom less then expectation.\n",i);
                        break;
		}
		
		iRet += n;	//记录实际读到的字节数作为函数返回值 		
	}

	usleep(10000);
	//读分段后的剩余部分
	if ( (i == seg_cnt) ) 
	{
		//定位
		if (lseek(fd, nOffest+(seg_cnt*512), SEEK_SET) < 0) {
			printf("eeprom locate error.\n"); 
			goto errexit;
		}
		//读取
		n = read(fd, pBuffer+(seg_cnt*512), remainder);
		if (n  < 0) { 
			printf("eeprom read error.\n");
			goto errexit;
		} 
        else if (n<remainder && n>=0){
			printf("In remainder read eeprom less then expectation.\n");
		}
		
		iRet += n;
	}

	flock(fd, LOCK_UN);
    close(fd);
	return iRet;

errexit:
    flock(fd, LOCK_UN);
    close(fd);
    return ERR_NORMAL;
}

/**********************************************************
* @brief 设置芯片写保护
* @param[in] wp_enable: 1: 打开写保护， 0: 关闭写保护
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
***********************************************************/
static int eeprom_write_protect_enable(unsigned char wp_enable)
{
    int32 fd = 0;
    int32 iRet = ERR_OK;
    
	//检验输入参数
	if (wp_enable != 1 && wp_enable != 0) {
    	DEBUG("WriteProtect param error(should be 1 or 0).\n");
    	return ERR_NORMAL;
	}

    fd = open(EEPROM_DEV_FILE, O_RDWR);
    if(fd == -1) 
    {
        DEBUG("eeprom dev open error\n");
        return ERR_NORMAL;
    }
    if(flock(fd, LOCK_EX))
    {
        DEBUG("Lock Error");
	   iRet = ERR_LOCK;
        goto end;
    }
    
	//写保护或去保护
    ioctl(fd, wp_enable);
    flock(fd, LOCK_UN);
end:

    close(fd);
    return iRet;
}

/**********************************************************
* @brief 读出许可
***********************************************************/
int read_lic_eeprom(char *appno, char *appname, char *license, int32 max_license_size)
{
    int  ret = 0;
    uint8 buf[4096] = {0};
    int i = 0;
    int namelen = 0;
    int index = 0;
    int num = 0;
    int datelen = 0;
    
    uint8 nobuf[24] = {0};
    uint8 nolen = 0;
    uint8 temp = 0;
    uint8 temp1 = 0;
    char  licbuf[72] = {0};

    for(i = 0; i < 48; i += 2)
    {
        if('0' <= appno[i] && appno[i] <= '9')
        {
            temp = (appno[i] - '0') * 16;
        }
        else if('A' <= appno[i] && appno[i] <= 'F')
        {
            temp = (appno[i] - 'A' + 10) * 16;
        }
        else if('a' <= appno[i] && appno[i] <= 'f')
        {
            temp = (appno[i] - 'a' + 10) * 16;
        }
        else
        {
            temp = 0;
        }

        if('0' <= appno[i + 1] && appno[i + 1] <= '9')
        {
            temp += appno[i + 1] - '0';
        }
        else if('A' <= appno[i + 1] && appno[i + 1] <= 'F')
        {
            temp += appno[i + 1] - 'A' + 10;
        }
        else if('a' <= appno[i + 1] && appno[i + 1] <= 'f')
        {
            temp += appno[i + 1] -'a' + 10;
        }
        else
        {
            temp = 0;
        }

        nobuf[nolen] = temp;
        nolen++;
    }
    
    ret = eeprom_read(buf, 4096, 4096);
    if(ret < 0)
    {
        memset(buf, 0, 4096);
    }
    if(buf[0] != buf[1] ||  buf[1] > 42)
    {
        memset(buf, 0, 4096);
    }
    num = buf[1];

    datelen = 64 + 96 * num;
    DEBUG_MSG(buf, datelen, "Read eeprom len %d:", datelen);
    for(i = 64; i < datelen; i++)
    {
        temp += buf[i];
    }
    if(temp != buf[63])
    {
        printf("eeprom data check failed\n");
        memset(buf, 0, 4096);
    }
    num = buf[1];
    
    for(index = 0; index < num; index++)
    {
        namelen = buf[+ 64 + 96 * index + 1];
        if(0 == memcmp(buf + 64 + 96 * index + 2 + namelen, nobuf, nolen))
        {
            break;
        }
    }

    /* 覆盖 */
    if(index < num)
    {
        memcpy(appname, buf + 64 + index * 96 + 2, namelen);
        for(i = 0; i < 36; i++)
        {
            temp = (buf[64 + index * 96 + 2 + namelen + 24 + i] & 0x0F);
            temp1 = ((buf[64 + index * 96 + 2 + namelen + 24 + i] >> 4) & 0x0F);
            licbuf[i * 2] = temp1 < 10 ? ('0' + temp1) : ('A' + temp1 - 10) ;
            licbuf[i * 2 + 1] = temp < 10 ? ('0' + temp) : ('A' + temp - 10) ;
        }
        memcpy(license, licbuf, max_license_size < 72 ? max_license_size : 72);
        ret = 0;
    }
    else
    {
        ret = ERR_NORESOURCE;
    }

    return ret;
}

/**********************************************************
* @brief 芯片写入许可
***********************************************************/
int write_lic_eeprom(const char *name, char *appno, char *applic, uint8 *fileBuf, uint16 buflen)
{
    int  ret = 0;
    uint8 buf[4096] = {0};
    int i = 0;
    int index = 0;
    uint16 datelen = 0;
    int num = 0;
    int   lenname = strlen(name);
    uint8 licbuf[96] = {0};
    uint8 licbuflen = 0;
    uint8 temp = 0;

    /* 先读许可数量 */
    ret = eeprom_read(buf, 4096, 96);
    if(ret < 0)
    {
        DEBUG("Read eeprom num ERROR\n");
        memset(buf, 0, 4096);
    }
    else
    {
        datelen = 64;
        DEBUG_MSG(buf, datelen, "Read eeprom len %d:", datelen);
        /* 校验长度 */
        if(buf[0] != buf[1] || buf[1] > 42)
        {
            memset(buf, 0, 4096);
        }
        else if(buf[1] > 0)
        {
            num = buf[1];
            datelen = 96 * num;
            ret = eeprom_read(buf + 96, 4096 + 96, datelen);
            if(ret < 0)
            {
                DEBUG("Read eeprom lic ERROR\n");
                memset(buf, 0, 4096);
            }
            else
            {
                DEBUG_MSG(buf + 96, datelen, "Read eeprom len %d:", datelen);
            }
        }
    }
    
    num = buf[1];
    datelen = 64 + 96 * num;
    for(i = 64; i < datelen; i++)
    {
        temp += buf[i];
    }
    if(temp != buf[63])
    {
        printf("eeprom data check failed\n");
        memset(buf, 0, 4096);
        num = buf[1];
    }
    for(index = 0; index < num; index++)
    {
        if(0 == memcmp(buf + 64 + 96 * index + 2, name, lenname >= 34 ? 34 : lenname))
        {
            break;
        }
    }
    licbuf[1] = lenname >= 32 ? 32 : lenname;
    memcpy(licbuf + 2, name, licbuf[1]);
    licbuflen = licbuf[1] + 2;
    for(i = 0; i < 48; i += 2)
    {
        if('0' <= appno[i] && appno[i] <= '9')
        {
            temp = (appno[i] - '0') * 16;
        }
        else if('A' <= appno[i] && appno[i] <= 'F')
        {
            temp = (appno[i] - 'A' + 10) * 16;
        }
        else if('a' <= appno[i] && appno[i] <= 'f')
        {
            temp = (appno[i] - 'a' + 10) * 16;
        }
        else
        {
            temp = 0;
        }

        if('0' <= appno[i + 1] && appno[i + 1] <= '9')
        {
            temp += appno[i + 1] - '0';
        }
        else if('A' <= appno[i + 1] && appno[i + 1] <= 'F')
        {
            temp += appno[i + 1] - 'A' + 10;
        }
        else if('a' <= appno[i + 1] && appno[i + 1] <= 'f')
        {
            temp += appno[i + 1] -'a' + 10;
        }
        else
        {
            temp = 0;
        }
        licbuf[licbuflen] = temp;
        licbuflen++;
    }
    for(i = 0; i < 72; i += 2)
    {
        if('0' <= applic[i] && applic[i] <= '9')
        {
            temp = (applic[i] - '0') * 16;
        }
        else if('A' <= applic[i] && applic[i] <= 'F')
        {
            temp = (applic[i] - 'A' + 10) * 16;
        }
        else if('a' <= applic[i] && applic[i] <= 'f')
        {
            temp = (applic[i] - 'a' + 10) * 16;
        }
        else
        {
            temp = 0;
        }

        if('0' <= applic[i + 1] && applic[i + 1] <= '9')
        {
            temp += applic[i + 1] - '0';
        }
        else if('A' <= applic[i + 1] && applic[i + 1] <= 'F')
        {
            temp += applic[i + 1] - 'A' + 10;
        }
        else if('a' <= applic[i + 1] && applic[i + 1] <= 'f')
        {
            temp += applic[i + 1] - 'a' + 10;
        }
        else
        {
            temp = 0;
        }
        licbuf[licbuflen] = temp;
        licbuflen++;
    }

    for(i = 1; i < licbuflen; i++)
    {
        licbuf[0] += licbuf[i];
    }
    DEBUG_MSG(licbuf, 96, "Write %s lic, %s-%s index %d:", name, appno, applic, index);

    /* 覆盖 */
    if(index < num || (index == num && num < 42))
    {
        memcpy(buf + 64 + index * 96, licbuf, 96);
        num = index == num ? (index + 1) : num;
        buf[0] = num;
        buf[1] = num;

        temp = 0;
        for(i = 64; i < 64 + 96 * num; i++)
        {
            temp += buf[i];
        }
        buf[63] = temp;

        datelen = 64 + 96 * num;
        eeprom_write_protect_enable(0);
        /* AT24驱动写入慢, 只写部分 */
        ret = eeprom_write(buf, 4096, 64);
        if(0 == ret)
        {
            ret = eeprom_write(licbuf, 4096 + 64 + 96 * index, 96);
        }
        //ret = eeprom_write(buf, 4096, datelen);
        eeprom_write_protect_enable(1);
        DEBUG_MSG(buf, datelen, "Write eeprom len %d:", datelen);
        memcpy(fileBuf, buf, datelen);
    }
    else
    {
        ret = ERR_NORMAL;
    }

    return ret;
}

/**********************************************************
* @brief 读出许可
***********************************************************/
int read_licnum_eeprom(uint8 *fileBuf, uint16 buflen)
{
    int  ret = 0;
    uint8 buf[4096] = {0};
    int   datelen = 0;
    int   i = 0;
    uint8 temp = 0;
    int num = 0;
    
    ret = eeprom_read(buf, 4096, 4096);
    if(ret < 0)
    {
        memset(buf, 0, 4096);
    }
    else
    {
        DEBUG_MSG(buf, ret, "Read eeprom data:");
    }
    
    if(buf[0] != buf[1] || buf[1] > 42)
    {
        memset(buf, 0, 4096);
        eeprom_write_protect_enable(0);
        ret = eeprom_write(buf, 4096, 4096);
        eeprom_write_protect_enable(1);
    }
    num = buf[1];

    datelen = 64 + num * 96;
    for(i = 64; i < datelen; i++)
    {
        temp += buf[i];
    }
    if(temp != buf[63])
    {
        printf("eeprom data check failed\n");
        memset(buf, 0, 4096);
        num = buf[1];
        eeprom_write_protect_enable(0);
        ret = eeprom_write(buf, 4096, 4096);
        eeprom_write_protect_enable(1);
    }
    DEBUG_MSG(buf, 64 + num * 96, "Read eeprom licnum %d: ", num);
    memcpy(fileBuf, buf, 4096);
    
    return num;
}

/**********************************************************
* @brief 读出许可
***********************************************************/
int read_lic_index_eeprom(int index, char *appno, char *appname, char *license, int32 max_license_size)
{
    int  ret = 0;
    uint8 buf[96] = {0};
    int i = 0;
    int namelen = 0;
    
    uint8 temp = 0;
    uint8 temp1 = 0;
    uint8 *start = NULL;
    char  licbuf[72] = {0};

    if(index > 42)
    {
        return ERR_NORESOURCE;
    }
    ret = eeprom_read(buf, 4096 + 64 + 96 * index, 96);
    if(ret < 0)
    {
        return ERR_NORESOURCE;
    }

    DEBUG_MSG(buf, 96, "Read eeprom lic index %d:", index);

    /* 验证 */
    for(i = 1; i < 96; i++)
    {
        temp += buf[i];
    }
    if(buf[0] != temp)
    {
        memset(buf, 0, 96);
        printf("eeprom lic check failed\n");
    }
    namelen = buf[1];

    if(0 == namelen || namelen > 32)
    {
        return ERR_NORESOURCE;
    }

    start = buf;
    memcpy(appname, start + 2, namelen);
    start += namelen + 2;
    for(i = 0; i < 24; i++)
    {
        temp = (start[i] & 0x0F);
        temp1 = ((start[i] >> 4) & 0x0F);
        appno[i * 2] = temp1 < 10 ? ('0' + temp1) : ('A' + temp1 - 10) ;
        appno[i * 2 + 1] = temp < 10 ? ('0' + temp) : ('A' + temp - 10) ;
    }
    start += 24;
    for(i = 0; i < 36; i++)
    {
        temp = (start[i] & 0x0F);
        temp1 = ((start[i] >> 4) & 0x0F);
        licbuf[i * 2] = temp1 < 10 ? ('0' + temp1) : ('A' + temp1 - 10) ;
        licbuf[i * 2 + 1] = temp < 10 ? ('0' + temp) : ('A' + temp - 10) ;
    }
    memcpy(license, licbuf, max_license_size < 72 ? max_license_size : 72);

    return 0;
}

/**********************************************************
* @brief 读出许可
***********************************************************/
int read_lic_file(char *devinfofile, char *appno, char *appname, char *license, int32 max_license_size)
{
    int   ret = 0;
    uint8 buf[4096] = {0};
    int   fd = 0;
    struct flock lock;
    
    int i = 0;
    int namelen = 0;
    int index = 0;
    int num = 0;
    int datelen = 0;

    uint8 nobuf[24] = {0};
    uint8 nolen = 0;
    uint8 temp = 0;
    uint8 temp1 = 0;
    char  licbuf[72] = {0};

    if(NULL == appno || NULL == appname || NULL == license)
    {
        return ERR_PTR;
    }

    for(i = 0; i < 48; i += 2)
    {
        if('0' <= appno[i] && appno[i] <= '9')
        {
            temp = (appno[i] - '0') * 16;
        }
        else if('A' <= appno[i] && appno[i] <= 'F')
        {
            temp = (appno[i] - 'A' + 10) * 16;
        }
        else if('a' <= appno[i] && appno[i] <= 'f')
        {
            temp = (appno[i] - 'a' + 10) * 16;
        }
        else
        {
            temp = 0;
        }

        if('0' <= appno[i + 1] && appno[i + 1] <= '9')
        {
            temp += appno[i + 1] - '0';
        }
        else if('A' <= appno[i + 1] && appno[i + 1] <= 'F')
        {
            temp += appno[i + 1] - 'A' + 10;
        }
        else if('a' <= appno[i + 1] && appno[i + 1] <= 'f')
        {
            temp += appno[i + 1] -'a' + 10;
        }
        else
        {
            temp = 0;
        }

        nobuf[nolen] = temp;
        nolen++;
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

    ret = fcntl(fd, F_SETLKW, &lock);
    if(ret != 0)
    {
        printf("Get id Return value of fcntl:%d\n",ret);
    }

    ret = read(fd, buf, 4096);
    
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    
    if(ret <= 0)
    {
        printf("active_id read error\n");
        return ERR_NORMAL;
    }

    if(buf[0] != buf[1] ||  buf[1] > 42)
    {
        memset(buf, 0, 4096);
    }
    num = buf[1];

    datelen = 64 + 96 * num;
    DEBUG_MSG(buf, datelen, "Read %s len %d:", devinfofile, datelen);
    for(i = 64; i < datelen; i++)
    {
        temp += buf[i];
    }
    if(temp != buf[63])
    {
        printf("%s lic data check failed\n", devinfofile);
        memset(buf, 0, 4096);
    }
    num = buf[1];
    
    for(index = 0; index < num; index++)
    {
        namelen = buf[+ 64 + 96 * index + 1];
        if(0 == memcmp(buf + 64 + 96 * index + 2 + namelen, nobuf, nolen))
        {
            break;
        }
    }

    /* 覆盖 */
    if(index < num)
    {
        memcpy(appname, buf + 64 + index * 96 + 2, namelen);
        for(i = 0; i < 36; i++)
        {
            temp = (buf[64 + index * 96 + 2 + namelen + 24 + i] & 0x0F);
            temp1 = ((buf[64 + index * 96 + 2 + namelen + 24 + i] >> 4) & 0x0F);
            licbuf[i * 2] = temp1 < 10 ? ('0' + temp1) : ('A' + temp1 - 10) ;
            licbuf[i * 2 + 1] = temp < 10 ? ('0' + temp) : ('A' + temp - 10) ;
        }
        memcpy(license, licbuf, max_license_size < 72 ? max_license_size : 72);
        ret = 0;
    }
    else
    {
        ret = ERR_NORESOURCE;
    }

    return ret;
}

int32 write_lic_file(char *devinfofile, uint8 *fileBuf, uint16 buflen)
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;

    if(NULL == devinfofile || NULL == fileBuf)
    {
        return ERR_PTR;
    }
    
    fd = open(devinfofile, O_CREAT|O_WRONLY,0666);
    if(fd < 0)
    {
        printf("open %s error!\n", devinfofile);
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
    iRet = write(fd, fileBuf, buflen);
    if(iRet != buflen)
    {
        printf("lic write file error\n");
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

/**********************************************************
* @brief 读出许可
***********************************************************/
int read_licnum_file(char *devinfofile, uint8 *fileBuf, uint16 buflen)
{
    int   ret = 0;
    uint8 buf[4096] = {0};
    int   fd = 0;
    struct flock lock;

    int   datelen = 0;
    int   i = 0;
    uint8 temp = 0;
    int   num = 0;

    if(NULL == devinfofile || NULL == fileBuf)
    {
        return ERR_PTR;
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

    ret = fcntl(fd, F_SETLKW, &lock);
    if(ret != 0)
    {
        printf("Get id Return value of fcntl:%d\n",ret);
    }
    ret = read(fd, buf, 4096);

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    if(ret <= 0)
    {
        printf("active_id read error\n");
        return ERR_NORMAL;
    }
    
    if(buf[0] != buf[1] || buf[1] > 42)
    {
        memset(buf, 0, 4096);
        write_lic_file(devinfofile, buf, 4096);
    }
    num = buf[1];

    datelen = 64 + num * 96;
    for(i = 64; i < datelen; i++)
    {
        temp += buf[i];
    }
    if(temp != buf[63])
    {
        printf("file data check failed\n");
        memset(buf, 0, 4096);
        num = buf[1];
        write_lic_file(devinfofile, buf, 4096);
    }
    DEBUG_MSG(buf, 64 + num * 96, "Read %s num %d: ", devinfofile, num);
    memcpy(fileBuf, buf, 4096);
    
    return num;
}

/**********************************************************
* @brief 读出许可
***********************************************************/
int read_lic_index_file(char *devinfofile, int index, char *appno, char *appname, char *license, int32 max_license_size)
{
    int  ret = 0;
    uint8 buf[96] = {0};
    int   fd = 0;
    struct flock lock;
    
    int i = 0;
    int namelen = 0;
    
    uint8 temp = 0;
    uint8 temp1 = 0;
    uint8 *start = NULL;
    char  licbuf[72] = {0};

    if(index > 42)
    {
        return ERR_NORESOURCE;
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

    ret = fcntl(fd, F_SETLKW, &lock);
    if(ret != 0)
    {
        printf("Get id Return value of fcntl:%d\n",ret);
    }
    
    lseek(fd, 64 + 96 * index, SEEK_SET);
    ret = read(fd, buf, 96);

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    if(ret <= 0)
    {
        printf("active_id read error\n");
        return ERR_NORMAL;
    }

    DEBUG_MSG(buf, 96, "Read %s lic index %d:", devinfofile, index);

    /* 验证 */
    for(i = 1; i < 96; i++)
    {
        temp += buf[i];
    }
    if(buf[0] != temp)
    {
        memset(buf, 0, 96);
        printf("eeprom lic check failed\n");
    }
    namelen = buf[1];

    if(0 == namelen || namelen > 32)
    {
        return ERR_NORESOURCE;
    }

    start = buf;
    memcpy(appname, start + 2, namelen);
    start += namelen + 2;
    for(i = 0; i < 24; i++)
    {
        temp = (start[i] & 0x0F);
        temp1 = ((start[i] >> 4) & 0x0F);
        appno[i * 2] = temp1 < 10 ? ('0' + temp1) : ('A' + temp1 - 10) ;
        appno[i * 2 + 1] = temp < 10 ? ('0' + temp) : ('A' + temp - 10) ;
    }
    start += 24;
    for(i = 0; i < 36; i++)
    {
        temp = (start[i] & 0x0F);
        temp1 = ((start[i] >> 4) & 0x0F);
        licbuf[i * 2] = temp1 < 10 ? ('0' + temp1) : ('A' + temp1 - 10) ;
        licbuf[i * 2 + 1] = temp < 10 ? ('0' + temp) : ('A' + temp - 10) ;
    }
    memcpy(license, licbuf, max_license_size < 72 ? max_license_size : 72);

    return 0;
}

#endif

/* 没有许可, 从临时文件读取 */
#if 1
int32 temp_read_applicense(const char *appno, char *appname, char *license, int32 max_license_size)
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;
    char   devinfofile[256] ={0};
    char   licdata[APPLIC_LEN * 2] = {0};
    char   noBig[256] = {0};
    int    offset = 0;
    int    len = 0;
    char   appAscii = 0;

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

    sprintf(devinfofile, "%s%s.lic", APP_TEMP_LIC_PATH, noBig);
    fd = open(devinfofile, O_CREAT|O_RDONLY,0666);
    if(fd <= 0)
    {
        return ERR_ACCESS;
    }

    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
    printf("Get id Return value of fcntl:%d\n",iRet);

    iRet = read(fd, licdata, APPLIC_LEN * 2); 
    if(iRet <= 0)
    {
        printf("active_id read error\n");
        iRet = ERR_NORMAL;
    }
    else
    {
        memcpy(license, licdata, (max_license_size >= iRet) ? iRet : max_license_size);
		license[(max_license_size >= iRet) ? iRet : max_license_size] =0;

        /* 读取名称 */
        lseek(fd, APPLIC_LEN * 2 + 1, SEEK_SET);
        len = 0;
        while(1)
        {
            iRet = read(fd, &appAscii, 1);
            if(1 != iRet)
            {
                break;
            }
            if(appAscii == ' ')
            {
                continue;
            }
            appname[len++] = appAscii;
        }
        /* 最后一个可能有问题 */
        if(('0' <= appname[len - 1] && appname[len - 1] <= '9') || 
           ('A' <= appname[len - 1] && appname[len - 1] <= 'Z') ||
           ('a' <= appname[len - 1] && appname[len - 1] <= 'z') )
        {
            appname[len] = 0;
        }
        else
        {
            appname[len - 1] = 0;
        }
        iRet = 0;
        printf("read %s applicense: %s\n", appname, license);
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return iRet;
    
}

uint32 temp_applicense_count()
{
    uint32 count = 0;
    DIR *dir = NULL;
    struct dirent *ptr = NULL;

    dir = opendir(APP_TEMP_LIC_PATH);
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

int32 temp_read_applicense_index(uint32 index, char *appno, char *appname, char *license, int32 max_license_size)
{
    uint32 count = 0;
    int32  ret = ERR_NOTEXIST;
    DIR   *dir = NULL;
    struct dirent *ptr = NULL;

    dir = opendir(APP_TEMP_LIC_PATH);
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
                    ret = temp_read_applicense(appno, appname, license, max_license_size);
                    break;
                }
            }
        }
    }

    closedir(dir);
    return ret;
}

#endif


/****************************************************************************************************
        *name    get_kernel_size
        *funciton 读取内核大小
        *param
        *return  -1 fail >0 内核大小
****************************************************************************************************/
int get_801_kernel_size(void)
{
	int fd = -1,ret=0,kernelsize=-1;
	fd = open(kernel_dev_name, O_RDONLY);
	if (fd < 0)
	{
		return kernelsize;
	}

	ret = read(fd,(char *)&kernel_header, sizeof(kernel_header));
	if( ret )
	{
		kernelsize  = ((kernel_header.ih_size>>24)&0xFF)<<0;
		kernelsize += ((kernel_header.ih_size>>16)&0xFF)<<8;
		kernelsize += ((kernel_header.ih_size>>8)&0xFF)<<16;
		kernelsize += ((kernel_header.ih_size>>0)&0xFF)<<24;
	}
	close(fd);
	return kernelsize + 64;
}
/****************************************************************************************************
        *name read_kernel_data
        *funciton 读取内核数据
        *param read_buf 读取内核数据的buffer
			   read_len 读取内核数据的大小
			   offset   读取内核数据起始偏移
        *return  -1 fail >0 实际读取大小
****************************************************************************************************/
int read_801_kernel_data(unsigned char *read_buf,int read_len,int offset)
{
	int fd = -1,readsize=-1;
	fd = open(kernel_dev_name, O_RDONLY);
	if (fd < 0)
	{
		return readsize;
	}
	lseek(fd,offset,SEEK_SET);
	readsize = read(fd,read_buf,read_len);
	return readsize;
}

/****************************************************************************************************
		*name	 get_bootloader_size
		*funciton 读取bootloader大小
		*param
		*return  -1 fail >0 bootloader大小
****************************************************************************************************/
int get_801_bootloader_size(void)
{
	int fd = -1,ret=0,ubootsize=-1;
	fd = open(uboot_dev_name, O_RDONLY);
	if (fd < 0)
	{
		return ubootsize;
	}
	ret = read(fd,(char *)&HeaderInfo, sizeof(HeaderInfo));
	if( ret )
	{
		ubootsize = HeaderInfo.content_len;
	}
	close(fd);
	return ubootsize + 128;
}
/****************************************************************************************************
		*name read_bootloader_data
		*funciton 读取uboot数据
		*param read_buf 读取uboot数据的buffer
			   read_len 读取uboot数据的大小
			   offset	读取uboot数据起始偏移
		*return  -1 fail >0 实际读取大小
****************************************************************************************************/
int read_801_bootloader_data(unsigned char * read_buf,int read_len,int offset)
{
	int fd = -1,readsize=-1;
	fd = open(uboot_dev_name, O_RDONLY);
	if (fd < 0)
	{
		return readsize;
	}
	lseek(fd,offset,SEEK_SET);
	readsize = read(fd,read_buf,read_len);
	return readsize;
}
/****************************************************************************************************
		*name	 read_bootloader_cert_id
		*funciton 读取BootLoader签名id
		*param	  cert_id 读取bootloader签名id的buffer
				  cert_id = NULL,获取bootloader签名id长度,
		*return  -1 fail >0 证书ID大小
****************************************************************************************************/
int read_801_bootloader_cert_id(unsigned char * cert_id)
{
	int fd = -1,ret=0,len=0,ubootsize=-1;
	unsigned char certiddata[100]={0};
	fd = open(uboot_dev_name, O_RDONLY);
	if (fd < 0)
	{
		return ubootsize;
	}
	ret = read(fd,(char *)&HeaderInfo, sizeof(HeaderInfo));
	if( ret )
	{
		ubootsize = HeaderInfo.content_len + 128;
	}
	lseek(fd,ubootsize,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5; //aaaa1111
	len += 68;
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5; //aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa6666
	len += 52;
	if(cert_id != NULL)
	{
		lseek(fd,ubootsize,SEEK_SET);
		read(fd,cert_id,len);
	}
	return len;
}
/****************************************************************************************************
		*name read_bootloader_sign
		*funciton 读取BootLoader签名
		*param cert_buf 读取签名数据的buffer
			   cert_buf = NULL,获取bootloader签名长度,
		*return  -1 fail >0 证书大小
****************************************************************************************************/
int read_801_bootloader_sign(unsigned char * cert_buf)
{
	int fd = -1,ret=0,len=0,ubootsize=-1;
	unsigned char certiddata[100];
	fd = open(uboot_dev_name, O_RDONLY);
	if (fd < 0)
	{
		return ubootsize;
	}
	ret = read(fd,(char *)&HeaderInfo, sizeof(HeaderInfo));
	if( ret )
	{
		ubootsize = HeaderInfo.content_len + 128;
	}
	lseek(fd,ubootsize,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5; //aaaa1111
	len += 68;
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5; //aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa6666
	len += 52;
	if(cert_buf != NULL)
	{
		lseek(fd,ubootsize+len,SEEK_SET);
		read(fd,cert_buf,64);
	}
	return 64;
}
/****************************************************************************************************
		*name	 read_kernel_cert_id
		*funciton 读取内核签名id
		*param	  cert_id 读取内核签名id的buffer
			      cert_id = NULL,获取内核签名id长度,
		*return  -1 fail >0 证书ID大小
****************************************************************************************************/
int read_801_kernel_cert_id(unsigned char * cert_id)
{
	int fd = -1,ret=0,len=0,ubootsize=-1;
	unsigned char certiddata[100]={0};
	fd = open(kernel_dev_name, O_RDONLY);
	if (fd < 0)
	{
		return ubootsize;
	}
	ret = read(fd,(char *)&kernel_header, sizeof(kernel_header));
	if( ret )
	{
		ubootsize  = ((kernel_header.ih_size>>24)&0xFF)<<0;
		ubootsize += ((kernel_header.ih_size>>16)&0xFF)<<8;
		ubootsize += ((kernel_header.ih_size>>8)&0xFF)<<16;
		ubootsize += ((kernel_header.ih_size>>0)&0xFF)<<24;
		ubootsize += 64;
	}
	lseek(fd,ubootsize,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5; //aaaa1111
	len += 68;
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5; //aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa6666
	len += 52;
	if(cert_id != NULL)
	{
		lseek(fd,ubootsize,SEEK_SET);
		read(fd,cert_id,len);
	}
	return len;
}
/****************************************************************************************************
		*name read_kernel_sign
		*funciton 读取内核签名
		*param cert_buf 读取内核签名数据的buffer
			   cert_buf = NULL,获取内核签名长度,
		*return  -1 fail >0 证书大小
****************************************************************************************************/
int read_801_kernel_sign(unsigned char * cert_buf)
{
	int fd = -1,ret=0,len=0,ubootsize=-1;
	unsigned char certiddata[100];
	fd = open(kernel_dev_name, O_RDONLY);
	if (fd < 0)
	{
		return ubootsize;
	}
	ret = read(fd,(char *)&kernel_header, sizeof(kernel_header));
	if( ret )
	{
		ubootsize  = ((kernel_header.ih_size>>24)&0xFF)<<0;
		ubootsize += ((kernel_header.ih_size>>16)&0xFF)<<8;
		ubootsize += ((kernel_header.ih_size>>8)&0xFF)<<16;
		ubootsize += ((kernel_header.ih_size>>0)&0xFF)<<24;
		ubootsize += 64;
	}
	lseek(fd,ubootsize,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5; //aaaa1111
	len += 68;
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5; //aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa2222
	lseek(fd,ubootsize + len,SEEK_SET);
	read(fd,(char *)&certiddata,12);
	len += (certiddata[1]<<8) + certiddata[2] + 5;//aaaa6666
	len += 52;
	if(cert_buf != NULL)
	{
		lseek(fd,ubootsize+len,SEEK_SET);
		read(fd,cert_buf,64);
	}
	return 64;
}

/**
* @brief读取内核大小
* @return返回内核大小。
*/
int32 devinfo_get_kernel_size(struct tag_DEVINFO_DEVICE *dev)
{
    return get_801_kernel_size();

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
    return read_801_kernel_data(read_buf, read_len, offset);
}

/**
* @brief读取bootloader大小
* @return返回bootloader大小。
*/
int32 devinfo_get_bootloader_size(struct tag_DEVINFO_DEVICE *dev)
{
    return get_801_bootloader_size();

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
    return read_801_bootloader_data(read_buf, read_len, offset);
}
/**
* @brief 读取BootLoader签名id
* @param[out] cert_id: 读取BootLoader签名id的缓冲区
cert_id = NULL,获取BootLoader签名id的长度
* @return成功返回大于0的BootLoader签名id的长度，失败返回错误码。
*/
int32 devinfo_read_bootloader_cert_id(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_id)
{
    return read_801_bootloader_cert_id(cert_id);
}
/**
* @brief 读取BootLoader签名
* @param[out] cert_buf: 读取签名数据的缓冲区
cert_buf = NULL,获取bootloader签名长度
* @return成功返回大于0的数据签名长度，失败返回错误码。
*/
int32 devinfo_read_bootloader_sign(struct tag_DEVINFO_DEVICE *dev, uint8 *cert_buf)
{
    return read_801_bootloader_sign(cert_buf);
}
/**
* @brief 读取内核签名id
* @param[out] cert_id: 读取内核签名id的缓冲区
cert_id = NULL,获取内核签名id的长度
* @return成功返回大于0的内核签名id的长度，失败返回错误码。
*/
int32 devinfo_read_kernel_cert_id(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_id)
{
    return read_801_kernel_cert_id(cert_id);
}
/**
* @brief 读取内核签名
* @param[out] cert_buf: 读取内核签名的缓冲区
cert_buf = NULL,获取内核签名的长度
* @return成功返回大于0的内核签名的长度，失败返回错误码。
*/
int32 devinfo_read_kernel_sign(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_buf)
{
    return read_801_kernel_sign(cert_buf);
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
    iRet = WriteIDFile(devinfofile, appid, appid_size);

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
    char   devinfofile[256] ={0};
    char   noBig[256] = {0};
    int    offset = 0;
    int    len = 0;

    if(appno == NULL || license == NULL || max_license_size <= 0)
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

    /* 没有许可从临时文件读取 */
    if(0 == access("/tmp/dev/not_verify_sign", F_OK))
    {
        return temp_read_applicense(noBig, appname, license, max_license_size);
    }

    /* 从铁电中读取 */
    if(0 == read_lic_eeprom(noBig, appname, license, max_license_size))
    {
        return 0;
    }

    /* 从文件中读取 */
    sprintf(devinfofile, "%slic.lic", APP_ID_STORE_PATH);
    if(0 == read_lic_file(devinfofile, noBig, appname, license, max_license_size))
    {
        return 0;
    }
    else
    {
        memset(devinfofile, 0, sizeof(devinfofile));
        sprintf(devinfofile, "%slic.lic", APP_ID_BACKUP_PATH);
        if(0 == read_lic_file(devinfofile, noBig, appname, license, max_license_size))
        {
            return 0;
        }
    }

    return ERR_NORMAL;
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
    char  licBig[256] = {0};
    int   offset = 0;
    int   len = 0;
    int32 status =  -1;
    uint8 buf[4096] = {0};
    uint8 bufcheck[4096] = {0};
    uint16 readlen = 0;

    if(appno == NULL || NULL == appname || NULL == license)
    {
        return ERR_PTR;
    }

    /* 写入clearapplic这个APP表明清空 */
    if(0 == strcmp(appname, "clear_applic"))
    {
        eeprom_write(bufcheck, 4096, 4096);
        sprintf(devinfofile, "rm -rf %slic.lic", APP_ID_STORE_PATH);
        system(devinfofile);
        
        memset(devinfofile, 0, sizeof(devinfofile));
        sprintf(devinfofile, "rm -rf %slic.lic", APP_ID_BACKUP_PATH);
        system(devinfofile);
        
        printf("clear app lic\n");
        return 0;
    }

    /* APP名称不能大于32 */
    if(strlen(appname) > 32)
    {
        return ERR_OVERFLOW;
    }
    
    if(access(APP_ID_STORE_PATH, F_OK) != 0)
    {
        status = mkdir(APP_ID_STORE_PATH, 0766);
        if(status < 0)
        {
            perror("mkdir /data/devinfo  error"); 
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

    for(offset = 0; offset < license_size; offset++)
    {
        if(('0' <= license[offset] && license[offset] <= '9') || ('A' <= license[offset] && license[offset] <= 'F'))
        {
            licBig[offset] = license[offset];
        }
        else if('a' <= license[offset] && license[offset] <= 'f')
        {
            licBig[offset] = 'A' + (license[offset] - 'a');
        }
        else
        {
            return ERR_FORMAT;
        }
    }

    /* 删除无许可标记 */
    if(0 == access("/tmp/dev/not_verify_sign", F_OK))
    {
        system("rm -f /tmp/dev/not_verify_sign");
        printf("rm -f /tmp/dev/not_verify_sign\n");
    }

    iRet = write_lic_eeprom(appname, noBig, licBig, buf, 4096);
    if(0 != iRet)
    {
        /* 铁电写入失败则返回错误 */
        printf("Write %s lic failed\n", appname);
        return iRet;
    }

    /* 写入成功再次获取一下铁电内容，防止铁电无效 */
    readlen = 64 + 96 * buf[0];
    if(readlen > 4096)
    {
        readlen = 4096;
    }
    
    iRet = eeprom_read(bufcheck, 4096, readlen);
    if(iRet < 0)
    {
        memset(bufcheck, 0, 4096);
    }
    if(0 != memcmp(buf, bufcheck, readlen))
    {
        printf("EEPROM ERROR\n");
        DEBUG_MSG(buf, 4096, "Write buf: ");
        DEBUG_MSG(bufcheck, 4096, "Check buf: ");
        return iRet;
    }

    sprintf(devinfofile, "%slic.lic", APP_ID_STORE_PATH);
    iRet = write_lic_file(devinfofile, bufcheck, 4096);
    if(0 != iRet)
    {
        printf("Lic write %s failed\n", devinfofile);
        return iRet;
    }
    memset(devinfofile, 0, sizeof(devinfofile));
    sprintf(devinfofile, "%slic.lic", APP_ID_BACKUP_PATH);
    iRet = write_lic_file(devinfofile, bufcheck, 4096);
    if(0 != iRet)
    {
        printf("Lic write %s failed\n", devinfofile);
        return iRet;
    }

    return iRet;
}

/** 
* @brief 当前 app 许可数量 
* @return 返回当前app许可数量
*/ 
uint32 devinfo_applicense_count( struct tag_DEVINFO_DEVICE *dev)
{
    uint32 count = 0;
    uint32 count1 = 0;
    uint32 count2 = 0;
    char   devinfofile1[256] ={0};
    char   devinfofile2[256] ={0};
    uint8  buf[4096] = {0};
    uint8  buf1[4096] = {0};
    uint8  buf2[4096] = {0};

    count = read_licnum_eeprom(buf, 4096);

    /* 没有许可从临时文件读取 */
    if(0 == count && 0 == access("/tmp/dev/not_verify_sign", F_OK))
    {
        return temp_applicense_count();
    }

    if(count > 0 && 0 == access("/tmp/dev/not_verify_sign", F_OK))
    {
        system("rm -f /tmp/dev/not_verify_sign");
    }
    
    sprintf(devinfofile1, "%slic.lic", APP_ID_STORE_PATH);
    count1 = read_licnum_file(devinfofile1, buf1, 4096);
    
    sprintf(devinfofile2, "%slic.lic", APP_ID_BACKUP_PATH);
    count2 = read_licnum_file(devinfofile2, buf2, 4096);

    if(count > 0)
    {
        /* 铁电有效 同步到文件 */
        if(0 != memcmp(buf, buf1, 4096))
        {
            write_lic_file(devinfofile1, buf, 4096);
        }
        if(0 != memcmp(buf, buf2, 4096))
        {
            write_lic_file(devinfofile2, buf, 4096);
        }
        return count;
    }
    else
    {
        /* 铁电无效 */
        if(count1 > 0)
        {
            eeprom_write_protect_enable(0);
            eeprom_write(buf1, 4096, 4096);
            eeprom_write_protect_enable(1);
            return count1;
        }
        else if(count2 > 0)
        {
            eeprom_write_protect_enable(0);
            eeprom_write(buf2, 4096, 4096);
            eeprom_write_protect_enable(1);

            write_lic_file(devinfofile1, buf2, 4096);
            return count2;
        }
    }

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
    char  devinfofile[256] ={0};

    if(appno == NULL || NULL == appname || license == NULL || max_license_size <= 0)
    {
        return ERR_PTR;
    }

    /* 没有许可从临时文件读取 */
    if(0 == access("/tmp/dev/not_verify_sign", F_OK))
    {
        return temp_read_applicense_index(index, appno, appname, license, max_license_size);
    }

    if(0 == read_lic_index_eeprom(index, appno, appname, license, max_license_size))
    {
        return 0;
    }
    
    sprintf(devinfofile, "%slic.lic", APP_ID_STORE_PATH);
    if(0 == read_lic_index_file(devinfofile, index, appno, appname, license, max_license_size))
    {
        return 0;
    }
    
    memset(devinfofile, 0, sizeof(devinfofile));
    sprintf(devinfofile, "%slic.lic", APP_ID_BACKUP_PATH);
    if(0 == read_lic_index_file(devinfofile, index, appno, appname, license, max_license_size))
    {
        return 0;
    }

    return ERR_NOTEXIST;
}


/**
* @brief 获取终端设备类型
* @return 成功返回 大于0的类型值。
          
0x1NXXXXXX表示集中器类终端（例如：0x11XXXXXX表示I型集中器、0x12XXXXXX表示II型集中器）
          
0x2NXXXXXX表示专变类终端（例如：0x21XXXXXX表示I型专变，0x22XXXXXX表示II型专变，0x23XXXXXX表示I
II型专变）
          0x3NXXXXXX表示能源控制器类终端（例如：0x31XXXXXX表示能源控制器专变）
          其中XXXXXX为厂家自定义，失败返回错误码
**/
int32 devinfo_get_device_type(struct tag_DEVINFO_DEVICE *dev)
{
    return 0x11FFFFFF;
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
    tempvalue = tempvalue * 10;
    fclose(pipe);
    return tempvalue;
}

/**
* @brief 获取厂商信息
* @return成功返回0；失败返回错误码。
*/
int32 devinfo_get_vendor_info(struct tag_DEVINFO_DEVICE *dev,char* szInfo, int infoSize)
{
    char *facinfo = "NJXL CCU 801";
    
    if((szInfo == NULL) || (infoSize < strlen(facinfo)+1))
    {
        return ERR_PTR;
    }
    
    strcpy(szInfo, facinfo);

    return 0;
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
    dev->devinfo_get_vendor_info = devinfo_get_vendor_info;
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


