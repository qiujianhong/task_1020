#include  <linux/fb.h>
#include  <lcd.h>
#include <gpio.h>
#include <linux/fb.h>
#include <sys/mman.h>



/*lcd 设备路径 */
#define   LCD_DEV_NAME_BK      "/dev/gpio"   /*背光控制 */
#define   LCD_DEV_NAME         "/dev/fb0"    /*frame buffer */
#define   ACTIVE_ID_FILE       "/tmp/dev/active_id" /*active_id 存储文件 */

#define LCD_DEV_NUM      1
static char *aLcdDevice[LCD_DEV_NUM] = {HW_DEVICE_ID_LCD};
#define   GPIO_BACKLIGHT	SCM_PIN_PD12   /*lcd背光管脚 */

/* 全局变量 */
uint32 *pfb_buffer = NULL;
struct fb_var_screeninfo varScreenInfo ;
struct fb_fix_screeninfo fixScreenInfo ;
int32 g_fb0_fd = 0;



/*********************************************
 所属功能单元: gpio
 功能描述:  set gpio status
 输入参数: 无
 输出参数: 无
 函数返回值: 无
 内部处理流程：all
**********************************************/
static int32 set_gpio_value(uint32 pin, uint32 value)
{        
    int32 fd;
    int32 ret_value;
    int32 count = 100;
	
    //打开
    while(((fd = open(GPIO_DEV_NAME, O_RDONLY))<0)&&((count--)>0))
            usleep(2000);
    
    if (fd < 0)
    {
      printf("can not open gpio device.\n");
      return ERR_NORMAL;
    }
    
    //(0 low | 1 high | <0 error)
    ret_value = ioctl(fd, value, pin);
    
    //关闭
    close(fd);
    
    return ret_value;             
										
}

/**
* @brief	   获取激活app的注册id号
* @param[in] dev: 设备描述
* @param[out] *active_id: 激活app的注册id号
* @return	   成功返回ERR_OK；失败返回错误码。详见：附录1
* @note 	   激活app的注册id号active_id存储在/tmp/dev/active_id文件下供所有接口共享。
*/
int32 lcd_active_id_get(struct tag_LCD_DEVICE *dev, uint32 *active_id)
{
   int32 fd = 0;
   int32 iRet= 0;
   struct flock lock;

   if(dev == NULL)
   	   return ERR_PTR;

   fd = open(ACTIVE_ID_FILE, O_CREAT|O_RDONLY,0666);
   if(fd < 0)
   {
	   DEBUG("open %s error!\n", ACTIVE_ID_FILE);
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
   
   *active_id = 20;
   iRet = read(fd, active_id, sizeof(uint32)); 

   if(iRet != sizeof(uint32))
   {
	   printf("active_id read error\n");
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
	* @brief	  设定激活app的注册id号
	* @param[in] dev: 设备描述
	* @param[in] active_id: 激活app的注册id号
	* @return	 成功返回ERR_OK；失败返回错误码。详见：附录1
	* @note 	 激活app的注册id号active_id存储在/tmp/dev/active_id文件下供所有接口共享
	*/
int32 lcd_active_id_set(struct tag_LCD_DEVICE *dev, uint32 active_id)
{
   int32 fd = 0;
   int32 iRet= 0;
   struct flock lock;

   if(dev == NULL)
   	 return ERR_PTR;

   printf("lcd_active_id_set app_regist_id %u,set active_id %u\n", dev->app_regist_id, active_id);

   uint32 local_id = 0;
   lcd_active_id_get(dev, &local_id);   
   if(dev->app_regist_id != 0 && dev->app_regist_id != local_id)
   {
        return ERR_PTR;
   }
   
     fd = open(ACTIVE_ID_FILE, O_CREAT|O_WRONLY,0666);
//    printf("12\n");
	 if(fd < 0)
	 {
	     DEBUG("open %s error!\n", ACTIVE_ID_FILE);
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

	iRet = write(fd, &active_id, sizeof(uint32)); 
	if(iRet != sizeof(uint32))
	 {
	    printf("active_id write error\n");
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
   //}
   //else
   ///{
   //  return ERR_NPERM;
   //}
}
	

/**
* @brief 背光控制
* @param[in] dev: 设备描述
* @param[in] bkl: 控制状态
* @return 成功返回TRUE；失败返回FALSE
*/
int32 lcd_bkl_set(struct tag_LCD_DEVICE *dev, LCD_BKL_TYPE_E eBkl)
{

    uint32 local_id = 0;
    lcd_active_id_get(dev, &local_id);   
    if(dev->app_regist_id != local_id)
    {
        return ERR_PTR;
    }

    printf("lcd_bkl_set app_regist_id %u \n", dev->app_regist_id);
   
    if(eBkl == LCD_BKL_OFF)
        set_gpio_value(GPIO_BACKLIGHT, GPIO_OUT_LOW);
    else if(eBkl == LCD_BKL_ON)
        set_gpio_value(GPIO_BACKLIGHT, GPIO_OUT_HIGH);

    return ERR_OK;
}

/**
* @brief 设置LCD 对比度
* @param[in] dev: 设备描述
* @param[in] ctrstlevel: 对比度值等级（0-15）， 由接口内部
				转换为具体LCD的对比度数值， 不同厂家、 不同LCD
				实现不同， 所以参数只定义等级， 从0到15， 对比度
				逐渐增大。
* @return 成功返回TRUE；失败返回FALSE
*/
int32 lcd_ctrstlevel_set(struct tag_LCD_DEVICE *dev, uint32 ctrstlevel)
{
  return ERR_OK;
}

/**
* @brief 设置LCD 灰度
* @param[in] dev: 设备描述
* @param[in] value 灰度值（0-3）
* @return 成功返回TRUE；失败返回FALSE
*/
int32 lcd_trayvalue_set(struct tag_LCD_DEVICE *dev, uint32 value)
{
  return ERR_OK;
}

/**
* @brief 设置LCD 温度补偿
* @param[in] dev: 设备描述
* @param[in] value 温度补偿值（0-3）
* @return 成功返回TRUE；失败返回FALSE
*/
int32 lcd_tempcompensation_set(struct tag_LCD_DEVICE *dev, uint32 value)
{
  return ERR_OK;
}


/**
* @brief		获取分辨率信息
* @param[in] dev: 设备描述
* @param[out]	pixel: 分辨率信息
* @return		成功返回TRUE；失败返回FALSE
*/
int32 lcd_info_get(struct tag_LCD_DEVICE *dev, TLCD_PIXEL_T *pixel)
{
    int iRet = 0;
    int fd = 0;
    struct fb_var_screeninfo fbinfo;

    if((NULL == dev)||(NULL == pixel))
    {
        printf("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return ERR_PTR;
    }
	
    fd = open(LCD_DEV_NAME, O_RDONLY);
    if (fd < 0) 
    { 
        printf("can not open device backlight.\n"); 
        return ERR_NORMAL; 
    }

    memset(&fbinfo, 0, sizeof(fbinfo));
    iRet = ioctl(fd,FBIOGET_VSCREENINFO,&fbinfo);
    if(0 != iRet)
    {
        printf("lcd get info error!\n");
        goto end;
    }
	
    pixel->pixel_row = fbinfo.xres;
    pixel->pixel_col = fbinfo.yres;
    
end:
    close(fd);
    return iRet;
}





/**
* @brief		刷新显存
* @param[in] dev: 设备描述
* @param[in]	(rowstart,colstart): 起始坐标
* @param[in]	(rowend,colend): 结束坐标
* @param[in]	buffer: 数据缓冲区
* @param[in]	size: 数据长度
* @return		成功返回TRUE；失败返回FALSE
*/
int32 lcd_brush_region(struct tag_LCD_DEVICE *dev, uint32 rowstart, uint32 colstart, 
      uint32 rowend, uint32 colend, const uint8 *buffer, uint32 size)
{

   int32 iRet = -1;
  /// int32 fd   = -1;
   int32 iRow = 0;
   int32 iRectWidth = 0;
   int32 iRectHight = 0;
   uint32 active_id = 0;
   uint8 *ptr = NULL;
   
   if((NULL == dev)||(NULL == buffer)||(size == 0))
   {
       printf("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
       return ERR_PTR;
   }
	 //阴阳码切换      2020-06-02
	int j;
   	uint8	databuf[20*160] = {0};
    uint8   tmp;                //字节序逆序 20200611

	if(size > 20*160)
	{
		printf("buffer oversized !\n");
		return ERR_O_RANGE;
	}

//    printf("buffer：");
	for(j = 0; j < size; j++)
	{
		databuf[j] = (~(buffer[j]));
        tmp = (databuf[j]<<7)&0x80;
        tmp += (databuf[j]<<5)&0x40;
        tmp += (databuf[j]<<3)&0x20;
        tmp += (databuf[j]<<1)&0x10;
        tmp += (databuf[j]>>1)&0x08;
        tmp += (databuf[j]>>3)&0x04;
        tmp += (databuf[j]>>5)&0x02;
        tmp += (databuf[j]>>7)&0x01;
        databuf[j] = tmp;

//        printf("%02X", databuf[j]);
	}

//    printf("!\n");
    
	if((rowstart > rowend)||( colstart > colend) 
						  ||(((colend - colstart+1)*(rowend - rowstart+1)/8) != size))
	{
	   printf("rowstart[%u],rowend[%u],colstart[%u],colend[%u],size[%u],!\n", rowstart, rowend, colstart, colend, size);
       printf("input row/col is error!\n");
	   return ERR_NORMAL;
	}

    iRet = lcd_active_id_get(dev, &active_id);
    if(iRet != 0)
		return ERR_NORMAL;

	if(active_id != dev->app_regist_id && dev->app_regist_id != 0)
	    return ERR_NPERM;

	
	if((colstart > varScreenInfo.xres) || (colend > varScreenInfo.xres) 
		                  || (rowstart > varScreenInfo.yres) 
		                  || (rowend   > varScreenInfo.yres))
	{
        return ERR_RANGE;
    }
						  
    iRectWidth = colend - colstart+1;
    iRectHight = rowend - rowstart+1;
	
	/* 判断起始点是否为整点*/
    if((colstart%8) == 0)
    {
       // printf("colstart%8 == 0\n");
    	for(iRow=0; iRow <iRectHight; iRow++)
    	{
    	    
    	    ptr = ((uint8 *)( pfb_buffer)) + 20*rowstart + colstart/8 + iRow*20;
            memcpy(ptr, databuf+(iRow*iRectWidth/8), iRectWidth/8);
           //// printf("irow %d ptr %x\n", iRow, *ptr);
    	}
    }
    else
    {
    	int ifbByte   = iRectWidth/8 + 1;
    	int ibuffByte = iRectWidth/8;
    	uint8 aTempBuff[20] = {0};
    	uint8 uShiftBits = colstart%8;

		///printf("colstart%8 != 0\n");
    
    	for(iRow=0; iRow <iRectHight; iRow++)
    	{   
    	    int i = 0;
    	    ptr = ((uint8 *)( pfb_buffer)) + 20*rowstart + colstart/8 + iRow*20;
    
    		aTempBuff[i] = (ptr[0]&(0xff>>(8-uShiftBits)))|(databuf[iRow*ibuffByte+i]&(0xff<<(uShiftBits)));
    		for(i=1; i< ibuffByte; i++)
    		{
    		   aTempBuff[i] = (databuf[iRow*ibuffByte+i-1]&(0xff>>(8-uShiftBits)))|(databuf[iRow*ibuffByte+i]&(0xff<<(uShiftBits)));
    		}	
    		
    		aTempBuff[i] = (databuf[iRow*ibuffByte+i-1]&(0xff>>(8-colstart%8)))|(ptr[i]&(0xff<<(colstart%8)));
            memcpy(ptr, aTempBuff, ifbByte);
    	}
    }    
    return ERR_OK;
}
	  
/**
* @brief		LCD复位控制
* @param[in] dev: 设备描述
* @return		成功返回TRUE；失败返回FALSE
*/
int32 lcd_reset(struct tag_LCD_DEVICE *dev)
{
  return ERR_OK;
}
      	
/**
* @brief		LCD异常状态检测
* @param[in] dev: 设备描述
* @param[out] val: 状态值， 正常: 1， 异常: 0
* @return		成功返回TRUE；失败返回FALSE
*/
int32 lcd_state_get(struct tag_LCD_DEVICE *dev, uint8 *val)
{
  return ERR_OK;
}
// 用户扩展部分


static int lcd_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
   
	int32 iRet = 0;
    
    LCD_DEVICE_T* dev = (LCD_DEVICE_T*)malloc(sizeof(LCD_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_MEM;
    }
    memset(dev, 0, sizeof(LCD_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_LCD;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->lcd_bkl_set  = lcd_bkl_set;
    dev->lcd_info_get = lcd_info_get;
    dev->lcd_ctrstlevel_set = lcd_ctrstlevel_set;
    dev->lcd_trayvalue_set  = lcd_trayvalue_set;
    dev->lcd_tempcompensation_set = lcd_tempcompensation_set;
    dev->lcd_brush_region = lcd_brush_region;
    dev->lcd_reset      = lcd_reset;
    dev->lcd_state_get  = lcd_state_get;
    dev->lcd_active_id_set = lcd_active_id_set;
    dev->lcd_active_id_get = lcd_active_id_get;
    
    *device = (HW_DEVICE*)dev;
	
    g_fb0_fd  = open(LCD_DEV_NAME, O_RDWR);
    if(g_fb0_fd < 0)
    {
        printf("open %s", LCD_DEV_NAME);
        close(g_fb0_fd);
        return ERR_NORMAL;
    }

	memset(&fixScreenInfo, 0x0, sizeof(fixScreenInfo));
    iRet = ioctl(g_fb0_fd , FBIOGET_FSCREENINFO, &fixScreenInfo);
    if(iRet < 0)
    {
        perror("ioctl");
        close(g_fb0_fd);
        return iRet;
    }

	memset(&varScreenInfo, 0x0, sizeof(varScreenInfo));
    iRet = ioctl(g_fb0_fd , FBIOGET_VSCREENINFO, &varScreenInfo);
    if(iRet < 0)
    {
        perror("ioctl");
        close(g_fb0_fd);
        return iRet;
    }

	printf("smem_len = %d\n",fixScreenInfo.smem_len);
    printf("xres         = %d;  yres         = %d\n",varScreenInfo.xres,varScreenInfo.yres);
    printf("xres_virtual = %d;  yres_virtual = %d\n",varScreenInfo.xres_virtual,varScreenInfo.yres_virtual);
    
    pfb_buffer = mmap(NULL, fixScreenInfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, g_fb0_fd ,0);

    if(pfb_buffer == NULL)
    {
       close(g_fb0_fd ); 
	   g_fb0_fd =-1;
	   printf("pfb_buffer == NULL\n");
       ///return ERR_PTR;
    }
	else
		printf("pfb_buffer = %p\n", pfb_buffer);

	memset(pfb_buffer , 0xff , fixScreenInfo.smem_len);
	////sleep(1);
    
    return ERR_OK;
}

static int lcd_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    LCD_DEVICE_T *dev = (LCD_DEVICE_T*)pDevice;

	if(dev == NULL)
		return ERR_PTR;
	
    free(dev);

    if(pfb_buffer == NULL)
		return ERR_PTR;
	
	/* unmap framebuffer*/
	munmap(pfb_buffer , fixScreenInfo.smem_len);

	if(g_fb0_fd > 0)
	    close(g_fb0_fd);

	return ERR_OK;
}

/* 初始化模块变量 */
LCD_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "lcd",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-20",
        .nDevNum = LCD_DEV_NUM,
        .szDevices = aLcdDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = lcd_open,
        .pfClose = lcd_close,
    },
};
