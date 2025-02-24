/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief：   错误信息定义
* @date：    2018/09/10
* @history： 
*********************************************************************
*/

#ifndef _ERR_CODE_H_
#define _ERR_CODE_H_

#ifdef __cplusplus
extern "C"{
#endif

/**接口函数错误代码*/
#define    ERR_OK              ( 0)    /**<成功*/
#define    ERR_NORMAL          (-1)    /**<一般性错误，未分类*/
#define    ERR_PTR             (-2)    /**<输入错误的指针值*/
#define    ERR_O_RANGE         (-3)    /**<数据超出有效范围*/
#define    ERR_LOCK            (-4)    /**<文件上存在不兼容的锁*/
#define    ERR_MEM             (-5)    /**<内存不够*/
#define    ERR_ACCESS          (-6)    /**<文件访问错误，一般指文件不存在*/
#define    ERR_TYPE            (-7)    /**<输入数据类型有误*/
#define    ERR_ITEM            (-8)    /**<数据项数组中不包含指定的数据项*/
#define    ERR_TIMEOUT         (-9)    /**<访问超时*/
#define    ERR_IFACE           (-10)   /**<接口函数出错*/
#define    ERR_NPERM           (-11)   /**<没有权限*/
#define    ERR_FEXISTS         (-12)   /**<文件已存在*/
#define    ERR_IO              (-13)   /**<读写文件或设备错误*/
#define    ERR_OPTION          (-14)   /**<错误的参数*/
#define    ERR_CHK             (-15)   /**<校验值错误*/
#define    ERR_DEV_NRDY        (-16)   /**<存储设备未就绪, Ramdisk, Eeprom, SD card, etc.*/
#define    ERR_UNINITED        (-17)   /**<动态库没有初始化*/
#define    ERR_IPCRES          (-18)   /**<进程间通讯资源出错*/
#define    ERR_FILEEXCEED      (-19)   /**<读写文件越界*/
#define    ERR_CLASS           (-20)   /**<类号读写出错（可能是读写文件出错，也可能当前id没有包含在所配置的类号中）*/
#define    ERR_NO_DIR          (-21)   /**<没有相关路径的文件*/
#define    ERR_READWR          (-22)   /**<写文件之前，读校验错误*/
#define    ERR_FAILPORT        (-23)   /**< 端口不存在*/
#define    ERR_PNULL           (-24)   /**< 操作指针为空*/
#define    ERR_EXECFAIL        (-25)   /**< 执行错误*/
#define    ERR_INVALID         (-26)   /**< 无效    */
#define    ERR_NORESOURCE      (-27)   /**< 没有资源*/
#define    ERR_WOULDBLOCK      (-28)   /**< 将会阻塞*/
#define    ERR_DUPLICATE       (-29)   /**< 重名*/
#define    ERR_NOTEXIST        (-30)   /**< 不存在*/
#define    ERR_APPNOTECHO      (-31)   /**< 应用没有回应*/
#define    ERR_NOCONFIG        (-32)   /**< 没有配置*/
#define    ERR_INVAL           (-33)   /**< 参数错误*/
#define    ERR_NOTDIR          (-34)   /**< 非法路径*/
#define    ERR_BADF            (-35)   /**< 文件编号错误*/
#define    ERR_NFILE           (-36)   /**< 文件列表溢出*/
#define    ERR_MFILE           (-37)   /**< 文件重复打开*/
#define    ERR_FBIG            (-38)   /**< 文件太大*/
#define    ERR_BUSY            (-39)   /**< 设备忙*/
#define    ERR_ACCES           (-40)   /**< 非法权限*/
#define    ERR_ADDRESS         (-41)   /**< 地址错误*/
#define    ERR_RANGE           (-42)   /**< 参数越界*/
#define    ERR_NODEV           (-43)   /**< 无效设备*/
#define    ERR_OVERFLOW        (-44)   /**< 溢出*/
#define    ERR_FORMAT          (-45)   /**< 非法格式*/
#define    ERR_UNCOMPATIBLE    (-46)   /**< 不兼容*/
#define    ERR_SOCBREAK        (-47)   /**< socket 连接断开*/
#define    ERR_NO_HANDLE       (-48)   /**< 句柄无效*/
#define    ERR_START_REC       (-49)   /**< 开始读记录数据返回失败*/
#define    ERR_SETP_REC        (-50)   /**< 单步读记录数据返回失败*/
#define    ERR_END_REC         (-51)   /**< 结束读记录数据返回失败*/
#define    ERR_BUF_CLEAR       (-52)   /**< 清接收缓存失败*/
#define    ERR_DISORDER        (-53)   /**< 消息乱序*/
#ifdef __cplusplus
}
#endif

#endif // _ERR_CODE_H_
