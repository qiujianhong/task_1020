
#ifdef AREA_JIANGXI

#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"
#include "area.h"
#include "hal.h"
#include "adc.h"
#include "power.h"
#include "../../Common698/AppCommon.h"
#include "../../event/Event.h"

//将版本信息以全局变量的形式写入各个地区的代码中，统一在 area.h 中引用
#ifdef ADV_PU_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< 厂商代码           */
    "GM01",      /**< 软件版本号         */
    "220304",    /**< 软件版本日期       */
    "DM01",      /**< 硬件版本号         */
    "200624",    /**< 硬件版本日期       */
    "GM01-14",   /**< 厂家扩展信息       */
};
#elif ADV_ST_AMR
FACTORY_VER_TMP_T g_tVer = 
{
    "XXXX",      /**< 厂商代码           */
    "ZM01",      /**< 软件版本号         */
    "220304",    /**< 软件版本日期       */
    "DM01",      /**< 硬件版本号         */
    "200624",    /**< 硬件版本日期       */
    "ZM01-14",   /**< 厂家扩展信息       */
};
#endif
OOP_DAR_E class11_nodetopology_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);



#endif

