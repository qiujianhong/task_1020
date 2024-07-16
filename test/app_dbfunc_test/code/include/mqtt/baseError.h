#ifndef __BASE_ERROR_H__
#define __BASE_ERROR_H__

#include <errno.h>

#ifdef __cplusplus
extern "C"
{

    //各模块使用不同区段的错误码
    //通用错误码使用LINUX错误码（正数） 补充错误码（负数）

    //TCP[-1000 ~ -1999]

    //UDP[-2000 ~ -2999]

    //MQTT[-3000 ~ -3999]

    //IPC[-4000 ~ -4999]

    //MSG[-5000 ~ -5999]

#ifdef __cplusplus
}
#endif

#endif