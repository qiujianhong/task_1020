#include "types.h"
#include <power.h>
#include <gpio.h>
#include "sysDefine.h"

#define POWER_DEV_NUM      1
static char *aPowerDevice[POWER_DEV_NUM] = {HW_DEVICE_ID_POWER};
/*power 设备路径 */
#define POWER_IO_FILE           "/dev/power_io"
/* The power ioctl argumet */
#define ARG_PH_EN_POW           2           //外接电池供电使能
#define ARG_RESET_PHY           4           //以太网电源控制


#if 0
static int usb_gpio[] = {
        -1,
        SCM_PIN_PH4, /* usb module 1 5V_EN1 */
        SCM_PIN_PH3, /* usb module 2 5V_EN2 */
        SCM_PIN_PB2, /* usb module 3 5V_EN3 */
        SCM_PIN_PB10, /* usb module 4 5V_EN1 */
        SCM_PIN_PH5, /* usb module 5 5V_EN1 */
        SCM_PIN_PB3, /* Bluetooth rst pin */
        SCM_PIN_PH6, /* usb hub VBUS_EN */
        SCM_PIN_PH7, /* usb hub reset */
};
#endif
#if 0
static int usb_gpio[] = {
        -1,
        SCM_PIN_PB10, /* usb module 1 5V_EN1 */
        SCM_PIN_PB2, /* usb module 2 5V_EN2 */
        SCM_PIN_PH3, /* usb module 3 5V_EN3 */
        SCM_PIN_PH4, /* usb module 4 5V_EN1 */
        SCM_PIN_PH5, /* usb module 5 5V_EN1 */
        SCM_PIN_PB3, /* Bluetooth rst pin */
        SCM_PIN_PH6, /* usb hub VBUS_EN */
        SCM_PIN_PH7, /* usb hub reset */
};
#endif

#define DDJC_PIN  SCM_PIN_PC17

/***********************************************
 * 函数功能: 电源控制模块接口定义
 * 输入参数: void
 * 输出参数: 0: 掉电检测正常，交采通电  -1:fail
 * 操作流程: 1、读交采电源标志
 ***********************************************/
int32 check_DDJC(void )
{
    int32 fd = -1;
    uint32 uData;
    int32 iRet = ERR_OK;
    
    /* open device file */
    fd = open(POWER_IO_FILE, O_RDWR);
    if(fd < 0)
    {
        perror("open power_io file error!");
        return ERR_NORMAL;
    }

    /* 读掉电检测状态 */
    if(read(fd, &uData, sizeof(uData)) 
        != sizeof(uData))
    {
       iRet = ERR_IO;
       goto end;
    }
    
    iRet = uData;
    
end:
  //  printf("power.data = %d\n", data);
    close(fd);
    return iRet;    
}


/*************************************************************************
 * 函数功能: 电源接口控制
 * 输入参数: cmd --> 0:disable  1:enable
 *           arg --> 控制变量
 * 输出参数: 0: success  -1:failure
 * 处理流程: 
 ************************************************************************/
int power_ioctl(unsigned int cmd, unsigned long arg)
{
    int fd = -1;

    fd = open(POWER_IO_FILE, O_RDWR);
    if(fd < 0)
    {
        perror("open power io control file error!");
        return ERR_NORMAL;
    }

    /* 控制电源 */
    ioctl(fd, cmd, arg);
    close(fd);
    
    return ERR_OK;
}


/*********************************************
 所属功能单元: gpio
 功能描述:  set gpio status
 输入参数: 无
 输出参数: 无
 函数返回值: 无
 内部处理流程：all
**********************************************/
int32 set_gpio_value(uint32 pin, uint32 value)
{        
    int32 fd;
    int32 iRet = ERR_OK;
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
    iRet = ioctl(fd, value, pin);
    
    //关闭?
    close(fd);

	iRet = (iRet >= 0)?ERR_OK : ERR_NORMAL;
    
    return iRet;             
										
}


/*********************************************
 所属功能单元: gpio
 功能描述:  set gpio status
 输入参数: 无
 输出参数: 无
 函数返回值: 无
 内部处理流程：all
**********************************************/
int32 get_gpio_value(uint32 pin)
{        
    int32 fd;
    int32 ret_value;
    int32 count = 100;
    int32 value = GPIO_IN_PULLUP;

    //打开
    while(((fd = open(GPIO_DEV_NAME, O_RDONLY))<0)&&((count--)>0))
            usleep(2000);
    
    if (fd < 0)
    {
      printf("can not open gpio device.\n");
      return ERR_NORMAL;
    }
    
    /* (0 low | 1 high | <0 error) */
    ret_value = ioctl(fd, value, pin);
    
    //关闭?
    close(fd);
    
    return ret_value;             

}

/**
* @brief 获取电源状态
* @param[in] dev: 设备描述
* @param[out] state: 电源状态，1有电，0 掉电
* @return 成功返回ERR_OK；失败返回错误编码
*/
int32 power_power_state_get(struct tag_POWER_DEVICE *dev, uint8 type, int32 *state)
{  
    int32 iRet = ERR_OK;
    //3个缓存, 同时为0则掉电
    static int32 s_Ret[3] = {1, 1, 1};

    iRet = check_DDJC();
    //老硬件处理 默认有电
    if(iRet<0)
    {
        iRet = 1;
    }
    s_Ret[0] = s_Ret[1];
    s_Ret[1] = s_Ret[2];
    s_Ret[2] = iRet;


    if(s_Ret[0] > 0 || s_Ret[1] > 0 || s_Ret[2] > 0)
    {
    	  *state = 1;
    }
    else if(s_Ret[0] == 0 && s_Ret[1] == 0 && s_Ret[2] == 0)
    {
       *state = 0;
    }

    return ERR_OK;
}


/**
* @brief 备用电源充电控制
* @param[in] dev: 设备描述
* @param[in] type: 0： 备用电池， 1： 法拉电容
* @param[in] val: 0： 不充电， 1： 充电
* @return 成功返回TRUE；失败返回FALSE
*/
int32 power_battery_charge_set(struct tag_POWER_DEVICE *dev, uint8 type, uint8 val)
{
	int32 iRet = ERR_OK;
	
	return iRet;
}


/**
* @brief 备用电源供电控制
* @param[in] dev: 设备描述
* @param[in] type: 0： 备用电池， 1： 法拉电容
* @param[in] val: 0： 关闭供电， 1： 供电
* @return 成功返回TRUE；失败返回FALSE
*/
int32 power_battery_out_set(struct tag_POWER_DEVICE *dev, uint8 type, uint8 val)
{
	int32 iRet = ERR_OK;

	power_ioctl(val, ARG_PH_EN_POW);  
	
	return iRet;
}


/**
* @brief 扩展模块电源控制
* @param[in] dev: 设备描述
* module_id 	  // 5个模块+1个蓝牙+usb hub	
* @param[in] val: 0： 关闭， 1： 开启
* @return 成功返回TRUE；失败返回FALSE
*/
int32 power_ply_vcc_out_set(struct tag_POWER_DEVICE *dev,uint8 module_id, uint8 val)
{
    int32 iRet = ERR_OK;

    switch(module_id)
    {
        case MODULE_1:
        case MODULE_2:
        case MODULE_3:
        case MODULE_4: 
        case MODULE_5:
        case MODULE_BT:
        case MODULE_USB_HUB:
    //        iRet = set_gpio_value(usb_gpio[module_id], (val>0?GPIO_OUT_HIGH:GPIO_OUT_LOW));
        break;

    default:
        iRet = ERR_NORMAL;
        break;
    }
	
    return iRet;

}

/**
* @brief 扩展模块电源控制
* @param[in] dev: 设备描述
* module_id       // 5个模块+1个蓝牙+usb hub  
* @param[in] val: 0： 关闭， 1： 开启
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32 power_ply_vcc_state_get(struct tag_POWER_DEVICE *dev,uint8 module_id, int32 *state)
{
    int32 iRet = ERR_OK;

    switch(module_id)
    {
        case MODULE_1:
        case MODULE_2:
        case MODULE_3:		 
        case MODULE_4:		 
        case MODULE_5:		 
        case MODULE_BT:
        case MODULE_USB_HUB:
           /// iRet = get_gpio_value(usb_gpio[module_id]);
        break;

    default:
        iRet = ERR_NORMAL;
        break;
    }
	
	return iRet;
}


static int power_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*)malloc(sizeof(POWER_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_MEM;
    }
	
    memset(dev, 0, sizeof(POWER_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_POWER;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->power_power_state_get  = power_power_state_get;
    dev->power_battery_charge_set = power_battery_charge_set;
	dev->power_battery_out_set  = power_battery_out_set;
#ifdef PRODUCT_ECU

    dev->power_ply_vcc_out_set = power_ply_vcc_out_set;
    dev->power_ply_vcc_out_set = power_ply_vcc_state_get;
#endif
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int power_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    POWER_DEVICE_T *dev = (POWER_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
POWER_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "power",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-20",
        .nDevNum = POWER_DEV_NUM,
        .szDevices = aPowerDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = power_open,
        .pfClose = power_close,
    },
};
