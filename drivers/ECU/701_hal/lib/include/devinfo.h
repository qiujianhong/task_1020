#ifndef _DEVINFO_H
#define _DEVINFO_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <string.h>
#include <hal.h>

#define HW_DEVICE_ID_DEVINFO   "devinfo"

/* 定义DEVINFO模块结构 */
typedef struct tag_DEVINFO_MODULE 
{
    HW_MODULE common;
}DEVINFO_MODULE;

typedef struct tag_DEVINFO_DEVICE
{
	
    struct tag_HW_DEVICE    base;

    /**
    * @brief        读取appid
    * @param[in]     appname: app英文名称
    * @param[out]    appid: 对应app的ID信息
    * @param[in]     max_appid_size: 存放appid的缓冲区大小
    * @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
    */
    int32 (*devinfo_read_appid)(struct tag_DEVINFO_DEVICE *dev, const char *appname, char *appid, int32 max_appid_size);


    /**
    * @brief        写appid
    * @param[in]     appname: app英文名称
    * @param[in]   appid: 对应app的ID信息
    * @param[in]    appid_size: appid的大小
    * @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
    */
    int32 (*devinfo_write_appid)(struct tag_DEVINFO_DEVICE *dev, const char *appname, const char *appid, int32 appid_size);

    /**
    * @brief 读取 APP 许可信息
    * @param[in] appno: app 编号
    * @param[out] license: 对应 app 的许可信息
    * @param[in] max_license_size: 存放 license 的缓冲区大小
    * @return 成功返回 ERR_OK；失败返回错误码。
    */
    int32 (*devinfo_read_applicense)(struct tag_DEVINFO_DEVICE *dev, const char *appno, char *license, int32 max_license_size);

    /**
    * @brief 写 app 许可信息
    * @param[in] appno: app 编号
    * @param[in] license: 对应 app 的许可信息
    * @param[in] license_size: app 许可信息的大小
    * @return 成功返回 ERR_OK；失败返回错误码。
    */
    int32 (*devinfo_write_applicense)(struct tag_DEVINFO_DEVICE *dev, const char *appno, const char *license, int32 license_size);

    
    /** 
    * @brief 当前 app 许可数量 
    * @return 返回当前app许可数量
    */ 
    uint32 (*devinfo_applicense_count)( struct tag_DEVINFO_DEVICE *dev);
    
    /** 
    * @brief 按序号读取 APP 许可信息 
    * @param[in] index: 序号（从0开始） 
    * @param[out] appno: 对应 app 的编号 
    * @param[out] license: 对应 app 的许可信息 
    * @param[in] max_license_size: 存放 license 的缓冲区大小 
    * @return 成功返回 ERR_OK；失败返回错误码。 
    */
    int32 (*devinfo_read_applicense_index)(struct tag_DEVINFO_DEVICE *dev, uint32 index, char *appno, char *license, int32 max_license_size); 
    

    // 用户扩展部分   
}DEVINFO_DEVICE_T;


#ifdef __cplusplus
}
#endif 


#endif
