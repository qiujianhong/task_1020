/*********************************************************************
 * Copyright(c) 2021,GW
 * 		All rights reserved.
 * 程序名称: ac.h
 * 版本号: 0.1
 * 功能: 交采采样接口说明
 * 其他: 
 *********************************************************************/

#ifndef _AC_H_
#define _AC_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include "filelock.h" 

typedef struct tagACSPI_COMM_PARA
{
  uint8  mode;               // SPI模式
  uint8  bits;               // 数据字的位数
  uint32 speed;              // 速率 按bit
  uint16 delay;              // 每个字间的延时
}ACSPI_COMM_PARA_T;

/**********************************************************************
* @name：      jc_reset
* @brief：     复位交采模块
* @param[in] ：delay_us    -  延时时间
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
extern int jc_reset(int delay_us);


/**********************************************************************
* @name：      ct_reset
* @brief：     复位交采模块
* @param[in] ：delay_us    -  延时时间
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
extern int ct_reset(int delay_us);


/**********************************************************************
* @name：      spi_mode_operation
* @brief：     模式设置
* @param[in] ：delay_us    -  延时时间
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
extern int spi_mode_operation(int32 fd, unsigned int mode, void *param);


/**********************************************************************
* @name：      ac_spi_open
* @brief：     打开交采设备枷锁
* @param[in] ：fp    -  设备句柄
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
extern int ac_spi_open(int* fp);

/**********************************************************************
* @name：      ac_spi_send_data
* @brief：     读取交采集数据
* @param[in] ：fp    -  设备句柄
               buf   -  接收缓存
               len   -  接收缓存大小
* @param[out]：
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
#if 0
extern int ac_spi_send_data(int32 fd, uint8 *buf, uint32 len);
#endif
int ac_spi_send_data(int32 fd, uint8 *txbuf, uint32 txlen, uint8 *rxbuf, uint32 frametime);

/**********************************************************************
* @name：      ac_spi_recv_data_timeout
* @brief：     接收
* @param[in] ：fd        -  句柄
               buflen    -  接收缓存最大范围   
               frametime -  帧超时时间 ms
               bytetime  -  字节超时时间 ms
* @param[out]：buf       -  接收缓存
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
#if 0
extern int ac_spi_recv_data_timeout(int32 fd, uint8 *buf, uint32 buflen, uint16 frametime, uint16 bytetime);
#endif
extern int ac_spi_recv_data_timeout(int32 fd, uint8 *txbuf, uint8 *rxbuf, uint16 buflen, uint32 frametime, uint32 bytetime);

/**********************************************************************
* @name：      ac_spi_read_data
* @brief：     读取交采集数据
* @param[in] ：fp    -  设备句柄
               len   -  接收缓存大小
* @param[out]：buf   -  接收缓存
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
#if 0
extern int ac_spi_read(int32 fd, uint8 *buf, uint16 len);
#endif
extern int ac_spi_read(int32 fd, uint8 *txbuf, uint8 *rxbuf, uint16 rxlen);

/**********************************************************************
* @name：      ac_spi_send_data
* @brief：     读取交采集数据
* @param[in] ：fp    -  设备句柄
               buf   -  接收缓存
               len   -  接收缓存大小
* @param[out]：
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
extern int ac_spi_send(int32 fd, uint8 *txbuf, uint32 txlen, uint8 *rxbuf);

/**********************************************************************
* @name：      ac_spi_close
* @brief：     关闭交采设备
* @param[in] ：fp    -  设备句柄
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
extern int ac_spi_close(int *fd);

/**********************************************************************
* @name：      ac_spi_init
* @brief：     设备初始化
* @param[in] ：para    -  参数
* @param[out]：
* @return    ：成功返回句柄；失败返回错误码。详见：附录1。
**********************************************************************/
extern int ac_spi_init(ACSPI_COMM_PARA_T *para);

#ifdef __cplusplus
}
#endif

#endif
