#ifndef _LED_H
#define _LED_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_LED  "led"
#define   LED_LIB_PATH      "/lib/hal_lib/libhal_led.so"  /*led 动态库路径 */

/*LED状态 */
typedef enum 
{
    LED_OFF = 0,
    LED_ON,
}LED_TYPE_E;
    
typedef enum         ///<运行模式
{
    COM_MODE = 0,
    FLICKER_MODE,
}MODE_TYPE_E;

typedef struct
{
    uint32 light_on_time;  ///<点亮时间,单位10ms
    uint32 light_off_time; ///<熄灭时间,单位10ms
}LED_ON_OFF_RATIO_E;


typedef enum
{
    EMLED_RUN = 1, //<运行灯
    EMLED_ALARM, //<告警灯
    EMLED_PC, //<功控灯
    EMLED_EC, //<电控灯
    EMLED_ES, //<保电灯
    EMLED_OL, //<在线灯
    EMLED_SIGG, //<信号强度绿灯
    EMLED_SIGR, //<信号强度红灯
    EMLED_DR, //<下行接收灯
    EMLED_DT, //<下行发送灯
    EMLED_UR, //<上行接收灯 
    EMLED_UT, //<上行发送灯
    EMLED_485IR, //< 485-1接收灯
    EMLED_485IT, //< 485-1发送灯
    EMLED_485IIR, //< 485-2接收灯
    EMLED_485IIT, //< 485-2发送灯
    EMLED_ROUND1G, //<轮次1绿灯
    EMLED_ROUND1R, //<轮次1红灯
    EMLED_ROUND2G, //<轮次2绿灯
    EMLED_ROUND2R, //<轮次2红灯
    EMLED_ROUND3G, //<轮次3绿灯
    EMLED_ROUND3R, //<轮次3红灯
    EMLED_ROUND4G, //<轮次4绿灯
    EMLED_ROUND4R, //<轮次4红灯
    EMLED_ROUND5G, //<轮次5绿灯
    EMLED_ROUND5R, //<轮次5红灯
    EMLED_ROUND6G, //<轮次6绿灯
    EMLED_ROUND6R, //<轮次6红灯
    EMLED_ROUND7G, //<轮次7绿灯
    EMLED_ROUND7R, //<轮次7红灯
    EMLED_ROUND8G, //<轮次8绿灯 
    EMLED_ROUND8R, //<轮次8红灯
    EMLED_RADIOTX, //<电台发送灯
    EMLED_RADIORX, //<电台接收灯
    EMLED_RADIOTEL, //<电台通话灯

}LED_INDEX_E;


/* 定义LED模块结构 */
typedef struct tag_LED_MODULE 
{
    HW_MODULE common;
}LED_MODULE;

/*定义LED设备结构 */
typedef struct tag_LED_DEVICE
{
    HW_DEVICE    base;
    
    /**
    * @param[in] dev: 设备描述
    * @param[in] index: LED灯序号
    * @param[in] state: LED灯状态
    * @param[in] mode: 配置模式（电平/闪烁模式）
    * @param[in] ratio: 亮灭时间(单位：10ms)
    * @param[in] flicker_count: 闪烁次数
    * @return 成功返回TRUE；失败返回FALSE
    */
    int (*led_state_set)(struct tag_LED_DEVICE *dev, LED_INDEX_E index, LED_TYPE_E state, 
    MODE_TYPE_E mode, LED_ON_OFF_RATIO_E *ratio, uint32 flicker_count);
}LED_DEVICE_T;


#ifdef __cplusplus
}
#endif 


#endif

