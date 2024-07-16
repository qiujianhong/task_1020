#ifndef _UART_H
#define _UART_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

/* 集中器定义串口(与ECU复用) */
#define UART_REMOTE_CU_DEVICE_ID_STR    "uart_remote_cu"    /* 远程通信单元串口  */
#define UART_RS4851_DEVICE_ID_STR       "uart_rs485_1"      /* 4851串口  */
#define UART_RS4852_DEVICE_ID_STR       "uart_rs485_2"      /* 4852串口  */
#define UART_RS4853_DEVICE_ID_STR       "uart_rs485_3"      /* 4853串口  */
#define UART_IR_DEVICE_ID_STR           "uart_ir"           /* 红外串口  */
#define UART_LOCAL_CU_DEVICE_ID_STR     "uart_local_cu"     /* 本地通信单元串口  */
#define UART_232_DEVICE_ID_STR          "uart_232"          /* 232串口  */
#define UART_RADIO_DEVICE_ID_STR        "uart_radio"        /* 电台串口 */
#define UART_USB_MANAGER_DEVICE_ID_STR  "uart_usb_manager"  /* 远程通信单元USB管理口 */
#define UART_USB_AT_DEVICE_ID_STR       "uart_usb_at"       /* 远程通信单元USB AT指令端口 */
#define UART_USB_DIAG_DEVICE_ID_STR     "uart_usb_diag"     /* 远程通信单元USB拨号口 */

/* 交采(集中器没有定义) */
#define UART_DEVICE_ACPORT_STR          "uart_acport"       /* 本地口 */

/* 蓝牙(集中器没有定义) */
#define UART_DEVICE_BLE_STR             "uart_ble"          /* 蓝牙口 */


/* 回路巡检(没有定义的接口) */
#define UART_DEVICE_LCM_STR             "uart_lcm"
#define UART_DEVICE_LJM_STR             "uart_ljm"


#define UART_ECU_DEVICE_ID_STR "uart_acm" /* 模块串口 */
/* 能源控制器串口 */
#define UART_1_0_DEVICE_ID_STR "uart_acm1_0" /* 模块1的管理串口 */
#define UART_1_1_DEVICE_ID_STR "uart_acm1_1" /* 模块1的串口1 */
#define UART_1_2_DEVICE_ID_STR "uart_acm1_2" /* 模块1的串口2 */
#define UART_1_3_DEVICE_ID_STR "uart_acm1_3" /* 模块1的串口3 */
#define UART_1_4_DEVICE_ID_STR "uart_acm1_4" /* 模块1的串口4 */

#define UART_2_0_DEVICE_ID_STR "uart_acm2_0" /* 模块2的管理串口 */
#define UART_2_1_DEVICE_ID_STR "uart_acm2_1" /* 模块2的串口1 */
#define UART_2_2_DEVICE_ID_STR "uart_acm2_2" /* 模块2的串口2 */
#define UART_2_3_DEVICE_ID_STR "uart_acm2_3" /* 模块2的串口3 */
#define UART_2_4_DEVICE_ID_STR "uart_acm2_4" /* 模块2的串口4 */

#define UART_3_0_DEVICE_ID_STR "uart_acm3_0" /* 模块3的管理串口 */
#define UART_3_1_DEVICE_ID_STR "uart_acm3_1" /* 模块3的串口1 */
#define UART_3_2_DEVICE_ID_STR "uart_acm3_2" /* 模块3的串口2 */
#define UART_3_3_DEVICE_ID_STR "uart_acm3_3" /* 模块3的串口3 */
#define UART_3_4_DEVICE_ID_STR "uart_acm3_4" /* 模块3的串口4 */

#define UART_4_0_DEVICE_ID_STR "uart_acm4_0" /* 模块4的管理串口 */
#define UART_4_1_DEVICE_ID_STR "uart_acm4_1" /* 模块4的串口1 */
#define UART_4_2_DEVICE_ID_STR "uart_acm4_2" /* 模块4的串口2 */
#define UART_4_3_DEVICE_ID_STR "uart_acm4_3" /* 模块4的串口3 */
#define UART_4_4_DEVICE_ID_STR "uart_acm4_4" /* 模块4的串口4 */

#define UART_5_0_DEVICE_ID_STR "uart_acm5_0" /* 模块5的管理串口 */
#define UART_5_1_DEVICE_ID_STR "uart_acm5_1" /* 模块5的串口1 */
#define UART_5_2_DEVICE_ID_STR "uart_acm5_2" /* 模块5的串口2 */
#define UART_5_3_DEVICE_ID_STR "uart_acm5_3" /* 模块5的串口3 */
#define UART_5_4_DEVICE_ID_STR "uart_acm5_4" /* 模块5的串口4 */

/* 双模组 目前不使用 */
#if 0
#define UART44_DEVICE_ID_STR "uart_acm1_1_0" /* 模块1子模块1的管理串口 */
#define UART45_DEVICE_ID_STR "uart_acm1_1_1" /* 模块1子模块1的串口1 */
#define UART46_DEVICE_ID_STR "uart_acm1_1_2" /* 模块1子模块1的串口2 */
#define UART47_DEVICE_ID_STR "uart_acm1_1_3" /* 模块1子模块1的串口3 */
#define UART48_DEVICE_ID_STR "uart_acm1_1_4" /* 模块1子模块1的串口4 */

#define UART49_DEVICE_ID_STR "uart_acm1_2_0" /* 模块1子模块2的管理串口 */
#define UART50_DEVICE_ID_STR "uart_acm1_2_1" /* 模块1子模块2的串口1 */
#define UART51_DEVICE_ID_STR "uart_acm1_2_2" /* 模块1子模块2的串口2 */
#define UART52_DEVICE_ID_STR "uart_acm1_2_3" /* 模块1子模块2的串口3 */
#define UART53_DEVICE_ID_STR "uart_acm1_2_4" /* 模块1子模块2的串口4 */

#define UART54_DEVICE_ID_STR "uart_acm2_1_0" /* 模块2子模块1的管理串口 */
#define UART55_DEVICE_ID_STR "uart_acm2_1_1" /* 模块2子模块1的串口1 */
#define UART56_DEVICE_ID_STR "uart_acm2_1_2" /* 模块2子模块1的串口2 */
#define UART57_DEVICE_ID_STR "uart_acm2_1_3" /* 模块2子模块1的串口3 */
#define UART58_DEVICE_ID_STR "uart_acm2_1_4" /* 模块2子模块1的串口4 */

#define UART59_DEVICE_ID_STR "uart_acm2_2_0" /* 模块2子模块2的管理串口 */
#define UART60_DEVICE_ID_STR "uart_acm2_2_1" /* 模块2子模块2的串口1 */
#define UART61_DEVICE_ID_STR "uart_acm2_2_2" /* 模块2子模块2的串口2 */
#define UART62_DEVICE_ID_STR "uart_acm2_2_3" /* 模块2子模块2的串口3 */
#define UART63_DEVICE_ID_STR "uart_acm2_2_4" /* 模块2子模块2的串口4 */

#define UART64_DEVICE_ID_STR "uart_acm3_1_0" /* 模块3子模块1的管理串口 */
#define UART65_DEVICE_ID_STR "uart_acm3_1_1" /* 模块3子模块1的串口1 */
#define UART66_DEVICE_ID_STR "uart_acm3_1_2" /* 模块3子模块1的串口2 */
#define UART67_DEVICE_ID_STR "uart_acm3_1_3" /* 模块3子模块1的串口3 */
#define UART68_DEVICE_ID_STR "uart_acm3_1_4" /* 模块3子模块1的串口4 */

#define UART69_DEVICE_ID_STR "uart_acm3_2_0" /* 模块3子模块2的管理串口 */
#define UART70_DEVICE_ID_STR "uart_acm3_2_1" /* 模块3子模块2的串口1 */
#define UART71_DEVICE_ID_STR "uart_acm3_2_2" /* 模块3子模块2的串口2 */
#define UART72_DEVICE_ID_STR "uart_acm3_2_3" /* 模块3子模块2的串口3 */
#define UART73_DEVICE_ID_STR "uart_acm3_2_4" /* 模块3子模块2的串口4 */

#define UART74_DEVICE_ID_STR "uart_acm4_1_0" /* 模块4子模块1的管理串口 */
#define UART75_DEVICE_ID_STR "uart_acm4_1_1" /* 模块4子模块1的串口1 */
#define UART76_DEVICE_ID_STR "uart_acm4_1_2" /* 模块4子模块1的串口2 */
#define UART77_DEVICE_ID_STR "uart_acm4_1_3" /* 模块4子模块1的串口3 */
#define UART78_DEVICE_ID_STR "uart_acm4_1_4" /* 模块4子模块1的串口4 */

#define UART79_DEVICE_ID_STR "uart_acm4_2_0" /* 模块4子模块2的管理串口 */
#define UART80_DEVICE_ID_STR "uart_acm4_2_1" /* 模块4子模块2的串口1 */
#define UART81_DEVICE_ID_STR "uart_acm4_2_2" /* 模块4子模块2的串口2 */
#define UART82_DEVICE_ID_STR "uart_acm4_2_3" /* 模块4子模块2的串口3 */
#define UART83_DEVICE_ID_STR "uart_acm4_2_4" /* 模块4子模块2的串口4 */

#define UART84_DEVICE_ID_STR "uart_acm5_1_0" /* 模块5子模块1的管理串口 */
#define UART85_DEVICE_ID_STR "uart_acm5_1_1" /* 模块5子模块1的串口1 */
#define UART86_DEVICE_ID_STR "uart_acm5_1_2" /* 模块5子模块1的串口2 */
#define UART87_DEVICE_ID_STR "uart_acm5_1_3" /* 模块5子模块1的串口3 */
#define UART88_DEVICE_ID_STR "uart_acm5_1_4" /* 模块5子模块1的串口4 */

#define UART89_DEVICE_ID_STR "uart_acm5_2_0" /* 模块5子模块2的管理串口 */
#define UART90_DEVICE_ID_STR "uart_acm5_2_1" /* 模块5子模块2的串口1 */
#define UART91_DEVICE_ID_STR "uart_acm5_2_2" /* 模块5子模块2的串口2 */
#define UART92_DEVICE_ID_STR "uart_acm5_2_3" /* 模块5子模块2的串口3 */
#define UART93_DEVICE_ID_STR "uart_acm5_2_4" /* 模块5子模块2的串口4 */
#endif

#define UART_LIB_PATH     "/lib/hal_lib/libhal_uart.so"

typedef enum
{
    PARITY_NO = 0,
    PARIGY_ODD,
    PARITY_EVEN,
} PARITY_TYPE_E;

typedef struct tag_UART_MODULE
{
    HW_MODULE common;
}UART_MODULE;

typedef struct tag_UART_DEVICE
{
  struct tag_HW_DEVICE    base;
  
  /**
  * @brief 设置串口通信参数
  * @param[in] dev: 设备描述
  * @param[in] baud 波特率
  * @param[in] databits 数据位(5~8)
  * @param[in] stopbits 停止位(1~2)
  * @param[in] parity 校验位(N 无校验，E 偶校验，O奇校验)
  * @return 成功返回TRUE；失败返回FALSE
  */
  int32 (*uart_param_set)(struct tag_UART_DEVICE *dev, uint32 baud, uint32 databits, 
  uint32 stopbits, PARITY_TYPE_E parity);
  
  /**
  * @brief 从串口接收数据
  * @param[in] dev: 设备描述
  * @param[out] buf 接收缓存区
  * @param[in] len 缓存区长度
  * @return 成功返回大于0的数据长度，失败返回FALSE
  */
  int32 (*uart_data_recv)(struct tag_UART_DEVICE *dev, uint8 *buf, uint32 len);

  /**
  * @brief 通过串口发送数据
  * @param[in] dev: 设备描述
  * @param[in] buf 发送缓存区
  * @param[in] len 缓存区长度
  * @return 成功返回TRUE；失败返回FALSE
  */
  int32 (*uart_data_send)(struct tag_UART_DEVICE *dev, const uint8 *buf, int32 len);

  /**
  * @brief 获取串口句柄
  * @param[in] dev: 设备描述
  * @return返回串口打开时的句柄值（正常>0，异常<=0）。
  */
  int32 (*uart_get_handle)(struct tag_UART_DEVICE *dev);

  /**
  * @brief 获取串口真实的设备节点名
  * @param[in] dev: 设备描述
  * @param[out] dev_name: 设备节点名
  * @return返回获取到的设备节点名的长度。
  */
  int32(*uart_get_real_dev_name)(struct tag_UART_DEVICE *dev, uint8 *dev_name);

  /**
  * @brief 获取串口状态
  * @param[in] dev: 设备描述
  * @return返回串口当前状态（正常>=0，异常<0）。
  */
  int32(*uart_get_status)(struct tag_UART_DEVICE *dev);

  // 用户扩展部分
  int32 fd;
  char  devName[48];
}UART_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

