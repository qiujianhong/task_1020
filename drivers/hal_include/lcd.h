#ifndef _LCD_H
#define _LCD_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_LCD  "lcd"
#define   LCD_LIB_PATH      "/lib/hal_lib/libhal_lcd.so"   /*lcd 动态库路径 */

/*背光状态 */
typedef enum 
{
        LCD_BKL_OFF = 0,
        LCD_BKL_ON
}LCD_BKL_TYPE_E;
        
/*分辨率信息 */
typedef struct tag_TLCD_PIXEL
{
    unsigned int pixel_row;      ///<行
    unsigned int pixel_col;      ///<列
} TLCD_PIXEL_T;
        

/* 定义LCD模块结构 */
typedef struct tag_LCD_MODULE 
{
    HW_MODULE common;
}LCD_MODULE;

/*定义LCD设备结构 */
typedef struct tag_LCD_DEVICE
{
    HW_DEVICE    base;

    /**
    * @brief 背光控制
    * @param[in] dev: 设备描述
    * @param[in] bkl: 控制状态
    * @return 成功返回TRUE；失败返回FALSE
    */
    int32 (*lcd_bkl_set)(struct tag_LCD_DEVICE *dev, LCD_BKL_TYPE_E bkl);
    
    /**
    * @brief 设置LCD 对比度
    * @param[in] dev: 设备描述
    * @param[in] ctrstlevel: 对比度值等级（0-15）， 由接口内部
                    转换为具体LCD的对比度数值， 不同厂家、 不同LCD
                    实现不同， 所以参数只定义等级， 从0到15， 对比度
                    逐渐增大。
    * @return 成功返回TRUE；失败返回FALSE
    */
    int32 (*lcd_ctrstlevel_set)(struct tag_LCD_DEVICE *dev, uint32 ctrstlevel);
    
    /**
    * @brief 设置LCD 灰度
    * @param[in] dev: 设备描述
    * @param[in] value 灰度值（0-3）
    * @return 成功返回TRUE；失败返回FALSE
    */
    int32 (*lcd_trayvalue_set)(struct tag_LCD_DEVICE *dev, uint32 value);
    
    /**
    * @brief 设置LCD 温度补偿
    * @param[in] dev: 设备描述
    * @param[in] value 温度补偿值（0-3）
    * @return 成功返回TRUE；失败返回FALSE
    */
    int32 (*lcd_tempcompensation_set)(struct tag_LCD_DEVICE *dev, uint32 value);
    
    
    /**
    * @brief        获取分辨率信息
    * @param[in] dev: 设备描述
    * @param[out]    pixel: 分辨率信息
    * @return        成功返回TRUE；失败返回FALSE
    */
    int32 (*lcd_info_get)(struct tag_LCD_DEVICE *dev, TLCD_PIXEL_T *pixel);
    /**
    * @brief        刷新显存
    * @param[in] dev: 设备描述
    * @param[in]    (rowstart,colstart): 起始坐标
    * @param[in]    (rowend,colend): 结束坐标
    * @param[in]    buffer: 数据缓冲区
    * @param[in]    size: 数据长度
    * @return        成功返回TRUE；失败返回FALSE
    */
    int32 (*lcd_brush_region)(struct tag_LCD_DEVICE *dev, uint32 rowstart, uint32 colstart, 
          uint32 rowend, uint32 colend, const uint8 *buffer, uint32 size);
    
    /**
    * @brief        LCD复位控制
    * @param[in] dev: 设备描述
    * @return        成功返回TRUE；失败返回FALSE
    */
    int32 (*lcd_reset)(struct tag_LCD_DEVICE *dev);
    /**
    * @brief        LCD异常状态检测
    * @param[in] dev: 设备描述
    * @param[out] val: 状态值， 正常: 1， 异常: 0
    * @return        成功返回TRUE；失败返回FALSE
    */
    int32 (*lcd_state_get)(struct tag_LCD_DEVICE *dev, uint8 *val);
    // 用户扩展部分

    /**
    * @brief      设定激活app的注册id号
    * @param[in] dev: 设备描述
    * @param[in] active_id: 激活app的注册id号
    * @return     成功返回ERR_OK；失败返回错误码。详见：附录1
    * @note      激活app的注册id号active_id存储在/tmp/dev/active_id文件下供所有接口共享
    */
    int32 (* lcd_active_id_set)(struct tag_LCD_DEVICE *dev, uint32 active_id);
    
    /**
    * @brief       获取激活app的注册id号
    * @param[in] dev: 设备描述
    * @param[out] *active_id: 激活app的注册id号
    * @return       成功返回ERR_OK；失败返回错误码。详见：附录1
    * @note        激活app的注册id号active_id存储在/tmp/dev/active_id文件下供所有接口共享。
    */
    int32 (* lcd_active_id_get)(struct tag_LCD_DEVICE *dev, uint32 *active_id);
    
    // 当注册ID和激活ID不一致时HAL不执行显示相关操作
    uint32 app_regist_id;         
    
    // 用户扩展部分    
}LCD_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

