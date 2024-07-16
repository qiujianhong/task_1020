#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "area.h"
#include "appcomm.h"
#include "oopservice.h"

//将版本信息以全局变量的形式写入各个地区的代码中，统一在 area.h 中引用
#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< 厂商代码           */
    "C201",      /**< 软件版本号         */
    "220623",    /**< 软件版本日期       */
    "CC01",      /**< 硬件版本号         */
    "220621",    /**< 硬件版本日期       */
    "C201-01",   /**< 厂家扩展信息       */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //能源控制器
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "E201",      /**< 软件版本号         */
        "220619",    /**< 软件版本日期       */
        "ER01",      /**< 硬件版本号         */
        "220619",    /**< 硬件版本日期       */
        "E201-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_1
    //专变1型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "1201",      /**< 软件版本号         */
        "220809",    /**< 软件版本日期       */
        "Z101",      /**< 硬件版本号         */
        "220727",    /**< 硬件版本日期       */
        "1201-03",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_2
    //专变2型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "2201",      /**< 软件版本号         */
        "220715",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "2201-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_3
    //专变3型
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "3201",      /**< 软件版本号         */
        "220816",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "01",   /**< 厂家扩展信息       */
    };
     #elif PRODUCT_ZCU_N
    //QT专变
    FACTORY_VER_TMP_T g_tVer = 
    {
        "XXXX",      /**< 厂商代码           */
        "N201",      /**< 软件版本号         */
        "210207",    /**< 软件版本日期       */
        "ZN01",      /**< 硬件版本号         */
        "200624",    /**< 硬件版本日期       */
        "N201-01",   /**< 厂家扩展信息       */
    };
    #endif
#endif


