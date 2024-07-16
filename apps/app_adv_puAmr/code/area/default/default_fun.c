#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "area.h"
#include "appcomm.h"
#include "oopservice.h"


//将版本信息以全局变量的形式写入各个地区的代码中，统一在 area.h 中引用
#ifdef BOARD_3358
#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer =
{
    "XXXX",      /**< 厂商代码           */
    "O1C1",      /**< 软件版本号         */
    "240629",    /**< 软件版本日期       */
    "CY01",      /**< 硬件版本号         */
    "220619",    /**< 硬件版本日期       */
    "O1C1-21",   /**< 厂家扩展信息       */
};
#elif ADV_ST_AMR
    #ifdef PRODUCT_ECU
    //能源控制器
    FACTORY_VER_TMP_T g_tVer =
    {
        "XXXX",      /**< 厂商代码           */
        "O1E1",      /**< 软件版本号         */
        "231123",    /**< 软件版本日期       */
        "EY01",      /**< 硬件版本号         */
        "220623",    /**< 硬件版本日期       */
        "O1E1-12",   /**< 厂家扩展信息        */
    };
    #elif PRODUCT_ZCU_1
    //专变
    FACTORY_VER_TMP_T g_tVer =
    {
        "XXXX",      /**< 厂商代码           */
        "O1Z1",      /**< 软件版本号         */
        "240701",    /**< 软件版本日期       */
        "Z101",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "O1Z1-15",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_2
    //专变2型
    FACTORY_VER_TMP_T g_tVer =
    {
        "XXXX",      /**< 厂商代码           */
        "Z201",      /**< 软件版本号         */
        "220715",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "Z201-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_3
    //专变3型
    FACTORY_VER_TMP_T g_tVer =
    {
        "XXXX",      /**< 厂商代码           */
        "O3Z1",      /**< 软件版本号         */
        "230831",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "O3Z1-09",   /**< 厂家扩展信息       */
    };
    #endif
#endif
#endif

#ifdef BOARD_801
    #ifdef PRODUCT_CCU
    FACTORY_VER_TMP_T g_tVer =
    {
        "XXXX",      /**< 厂商代码           */
        "O1C2",      /**< 软件版本号*/
        "240715",    /**< 软件版本日期       */
        "CY02",      /**< 硬件版本号         */
        "240206",    /**< 硬件版本日期       */
        "O1C2-09",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ECU
    //能源控制器
    FACTORY_VER_TMP_T g_tVer =
    {
        "XXXX",      /**< 厂商代码           */
        "V1.2",      /**< 软件版本号         */
        "240525",    /**< 软件版本日期       */
        "EY02",      /**< 硬件版本号         */
        "240312",    /**< 硬件版本日期       */
        "O1E2-07",   /**< 厂家扩展信息        */
    };
    #elif PRODUCT_ZCU_1
    //专变
    FACTORY_VER_TMP_T g_tVer =
    {
        "XXXX",      /**< 厂商代码           */
        "O1Z1",      /**< 软件版本号         */
        "230831",    /**< 软件版本日期       */
        "Z101",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "O1Z1-15",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_2
    //专变2型
    FACTORY_VER_TMP_T g_tVer =
    {
        "XXXX",      /**< 厂商代码           */
        "Z201",      /**< 软件版本号         */
        "220715",    /**< 软件版本日期       */
        "Z301",      /**< 硬件版本号         */
        "220715",    /**< 硬件版本日期       */
        "Z201-01",   /**< 厂家扩展信息       */
    };
    #elif PRODUCT_ZCU_3
    //专变3型
    FACTORY_VER_TMP_T g_tVer =
    {
        "XXXX",      /**< 厂商代码           */
        "O3Z2",      /**< 软件版本号         */
        "240715",    /**< 软件版本日期       */
        "Z302",      /**< 硬件版本号         */
        "240206",    /**< 硬件版本日期       */
        "O3Z2-11",   /**< 厂家扩展信息       */
    };
    #endif
#endif

