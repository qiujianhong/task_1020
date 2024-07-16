#ifndef _DEVINFO_H
#define _DEVINFO_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <string.h>
#include <hal.h>


#define HW_DEVICE_ID_DEVINFO   "devinfo"

/* 瀹氫箟DEVINFO妯″潡缁撴瀯 */
typedef struct tag_DEVINFO_MODULE 
{
    HW_MODULE common;
}DEVINFO_MODULE;


typedef struct tag_DEVINFO_DEVICE
{
    struct tag_HW_DEVICE    base;
    
    /**
    * @brief读取appid
    * @param[in] appname: app英文名称
    * @param[out] appid: 对应app的ID信息
    * @param[in] max_appid_size: 存放appid的缓冲区大小
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*devinfo_read_appid)(struct tag_DEVINFO_DEVICE *dev, const char *appname, char *appid, int32 max_appid_size);

    /**
    * @brief写appid
    * @param[in] appname: app英文名称
    * @param[in] appid: 对应app的ID信息
    * @param[in] appid_size: appid的大小
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*devinfo_write_appid)(struct tag_DEVINFO_DEVICE *dev, const char *appname, const char *appid, int32 appid_size);

    /**
    * @brief读取内核大小
    * @return返回内核大小。
    */
    int32 (*devinfo_get_kernel_size)(struct tag_DEVINFO_DEVICE *dev);

    /**
    * @brief 读取内核数据
    * @param[out] read_buf: 读取内核数据的buffer
    * @param[in] read_len: 读取一定大小的内核数据
    * @param[in] offset: 读取内核数据的起始偏移
    * @return成功返回大于0的数据长度，失败返回错误码。
    */
    int32 (*devinfo_read_kernel_data)(struct tag_DEVINFO_DEVICE *dev, uint8 *read_buf, uint32 read_len, uint32 offset);

    /**
    * @brief读取bootloader大小
    * @return返回bootloader大小。
    */
    int32 (*devinfo_get_bootloader_size)(struct tag_DEVINFO_DEVICE *dev);

    /**
    * @brief 读取bootloader数据
    * @param[out] read_buf: 读取bootloader数据的buffer
    * @param[in] read_len: 读取一定大小的bootloader数据
    * @param[in] offset: 读取bootloader数据定的起始偏移
    * @return成功返回大于0的数据长度，失败返回错误码。
    */
    int32 (*devinfo_read_bootloader_data)(struct tag_DEVINFO_DEVICE *dev, uint8 *read_buf, uint32 read_len, uint32 offset);

    /**
    * @brief 读取BootLoader签名id
    * @param[out] cert_id: 读取BootLoader签名id的缓冲区    cert_id = NULL,获取BootLoader签名id的长度
    * @return成功返回大于0的BootLoader签名id的长度，失败返回错误码。
    */
    int32 (*devinfo_read_bootloader_cert_id)(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_id);

    /**
    * @brief 读取BootLoader签名
    * @param[out] cert_buf: 读取签名数据的缓冲区    cert_buf = NULL,获取bootloader签名长度
    * @return成功返回大于0的数据签名长度，失败返回错误码。
    */
    int32 (*devinfo_read_bootloader_sign)(struct tag_DEVINFO_DEVICE *dev, uint8 *cert_buf);

    /**
    * @brief 读取内核签名id
    * @param[out] cert_id: 读取内核签名id的缓冲区    cert_id = NULL,获取内核签名id的长度
    * @return成功返回大于0的内核签名id的长度，失败返回错误码。
    */
    int32 (*devinfo_read_kernel_cert_id)(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_id);

    /**
    * @brief 读取内核签名
    * @param[out] cert_buf: 读取内核签名的缓冲区    cert_buf = NULL,获取内核签名的长度
    * @return成功返回大于0的内核签名的长度，失败返回错误码。
    */
    int32 (*devinfo_read_kernel_sign)(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_buf);

    /** 
    * @brief 读取 APP 许可信息 
    * @param[in] appno: app 编号
    * @param[out] appname: 对应 app 的名称 
    * @param[out] license: 对应 app 的许可信息 
    * @param[in] max_license_size: 存放 license 的缓冲区大小 
    * @return 成功返回 ERR_OK；失败返回错误码。 
    */
    int32 (*devinfo_read_applicense)(struct tag_DEVINFO_DEVICE *dev, const char *appno, char *appname, char *license, 
                                     int32 max_license_size); 

    /** 
    * @brief 写 app 许可信息 
    * @param[in] appno: app 编号 
    * @param[in] license: 对应 app 的许可信息 
    * @param[in] license_size: app 许可信息的大小 
    * @return 成功返回 ERR_OK；失败返回错误码。 
    */ 
    int32 (*devinfo_write_applicense)(struct tag_DEVINFO_DEVICE *dev, const char *appno, const char *appname, 
                                      const char *license, int32 license_size);

    /** 
    * @brief 当前 app 许可数量 
    * @return 返回当前app许可数量
    */ 
    uint32 (*devinfo_applicense_count)( struct tag_DEVINFO_DEVICE *dev);

    /** 
    * @brief 按序号读取 APP 许可信息 
    * @param[in] index: 序号（从0开始） 
    * @param[out] appno: 对应 app 的编号 
    * @param[out] appname: 对应 app 的名称
    * @param[out] license: 对应 app 的许可信息 
    * @param[in] max_license_size: 存放 license 的缓冲区大小 
    * @return 成功返回 ERR_OK；失败返回错误码。 
    */
    int32 (*devinfo_read_applicense_index)(struct tag_DEVINFO_DEVICE *dev, uint32 index, char *appno, char *appname, 
                                           char *license, int32 max_license_size);

    /**
    * @brief 获取终端设备类型
    * @return 成功返回 大于0的类型值。
              0x1NXXXXXX表示集中器类终端（例如：0x11XXXXXX表示I型集中器、0x12XXXXXX表示II型集中器）
              0x2NXXXXXX表示专变类终端（例如：0x21XXXXXX表示I型专变，0x22XXXXXX表示II型专变，0x23XXXXXX表示III型专变）
              0x3NXXXXXX表示能源控制器类终端（例如：0x31XXXXXX表示能源控制器专变）
              其中XXXXXX为厂家自定义，失败返回错误码
    **/
    int32 (*devinfo_get_device_type)(struct tag_DEVINFO_DEVICE *dev);

    /**
    * @brief 获取终端CPU温度
    * @return成功返回温度值，单位0.1℃；失败返回错误码。
    */
    int32 (*devinfo_get_cpu_temperature)(struct tag_DEVINFO_DEVICE *dev);

    /**
    * @brief 获取厂商信息
    * @return成功返回0；失败返回错误码。
    */
    int32 (*devinfo_get_vendor_info)(struct tag_DEVINFO_DEVICE *dev,char* szInfo, int infoSize);

    // 用户扩展部分  

}DEVINFO_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif
