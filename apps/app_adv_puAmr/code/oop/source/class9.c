/*
*********************************************************************
* @file    class9.c
* @brief： class9处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "baseCommon.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "commClass.h"
#include "oopStorage.h"

//
// class9
//
const OOP_OAD_U g_tg50021min_objs[]={//总加组曲线
    {0x23010300},
    {0x23010400},
};

const OOP_OAD_U g_tg5002_objs[]={//总加组曲线
    {0x23010300},
    {0x23010400},
    {0x23010700},
    {0x23010800},
};

const OOP_OAD_U g_tg5004_objs[]={//总加组日冻结
    {0x23010700},
    {0x23010800},
};

const OOP_OAD_U g_tg5006_objs[]={//总加组月冻结
    {0x23010900},
    {0x23010A00},
};

const OOP_OAD_U g_pulse50021min_objs[]={//脉冲曲线
    {0x24010500},
    {0x24010600},
};

const OOP_OAD_U g_pulse5002_objs[]={//脉冲曲线
    {0x24010500},
    {0x24010600},
    {0x24010700},
    {0x24010900},
    {0x24010B00},
    {0x24010D00},
    {0x24010F00},
};

const OOP_OAD_U g_pulse5004_objs[]={//脉冲日冻结
    {0x24010700},
    {0x24010900},
    {0x24010B00},
    {0x24010D00},
};

const OOP_OAD_U g_pulse5006_objs[]={//脉冲月冻结
    {0x24010800},
    {0x24010A00},
    {0x24010C00},
    {0x24010E00},
};

const OOP_OAD_U g_ac5002_objs[]={//交采分钟冻结
    {0x00100200},
    {0x00110200},
    {0x00120200},
    {0x00130200},
    {0x00200200},
    {0x00300200},
    {0x00400200},
    {0x20000200},
    {0x20010200},
    {0x20020200},
    {0x20040200},
    {0x20050200},
    {0x200A0200},
    {0x26000200},  //温度
    {0x26010200},  //湿度
#ifdef AREA_HUNAN
    {0x2A030201},  //A相变压器桩头温度
    {0x2A030202},  //B相变压器桩头温度
    {0x2A030203},  //C相变压器桩头温度
#endif
};

const OOP_OAD_U g_ac5003_objs[]={//交采小时冻结
    {0x00100200},
    {0x00110200},
    {0x00120200},
    {0x00130200},
};

const OOP_OAD_U g_ac5004_objs[]={//交采日冻结
    {0x00100200},
    {0x00110200},
    {0x00120200},
    {0x00130200},
    {0x00200200},
    {0x00300200},
    {0x00400200},
    {0x00500200},
    {0x00600200},
    {0x00700200},
    {0x00800200},
    {0x10100200},
    {0x10200200},
    {0x10300200},
    {0x10400200},

};
    
const OOP_OAD_U g_ac5006_objs[]={//交采月冻结
    {0x00100200},
    {0x00110200},
    {0x00120200},
    {0x00130200},   
    {0x00200200},
    {0x00300200},
    {0x00400200},
    {0x00500200},
    {0x00600200},
    {0x00700200},
    {0x00800200},
    {0x10100200},
    {0x10200200},
};

#if DESC("默认冻结配置",1)

/**
*********************************************************************
* @name：       frozen_cfg_cmp
* @brief：      比较冻结参数是否相同
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
int frozen_cfg_cmp(void *vl, void *vr)
{
    OOP_FROZENOBJ_T *lvalue = (OOP_FROZENOBJ_T *)vl;
    OOP_FROZENOBJ_T *rvalue = (OOP_FROZENOBJ_T *)vr;

    if(lvalue->oad.value == rvalue->oad.value)
    {
        return 0;
    }

    return -1;
}

/**
*********************************************************************
* @name：       frozen_objattr_batch_add
* @brief：      批量添加冻结对象属性
* @param[in] ：fd           -数据中心句柄
               frzOAD      -冻结主OAD
               frzSets     -冻结关联OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_data_clear(DB_CLIENT fd, OOP_OAD_U frzOAD)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData));
    
    //清除冻结记录
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = frzOAD.value;
    clearData.recordOAD.road.oadCols.oad[0].attID = 2;
    
    ret = db_clear_oad_data(fd, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return ret;
    }

    return ret;
}

/**
*********************************************************************
* @name：       frozen_objattr_batch_add
* @brief：      批量添加冻结对象属性
* @param[in] ：fd           -数据中心句柄
               frzOAD      -冻结主OAD
               frzSets     -冻结关联OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_objattr_clear(DB_CLIENT fd, OOP_OAD_U frzOAD)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData));
    
    //清除冻结记录
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = frzOAD.value;
    
    ret = db_clear_oad_data(fd, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return ret;
    }

    return ret;
}

/**
*********************************************************************
* @name：       frozen_objattr_batch_add
* @brief：      批量添加冻结对象属性
* @param[in] ：fd           -数据中心句柄
               frzOAD      -冻结主OAD
               frzSets     -冻结关联OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_objattr_batch_add(DB_CLIENT fd, OOP_OAD_U frzOAD, OOP_FROZENSETS_T *frzSets)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    OOP_FROZENSETS_T data;
    OOP_FROZENSETS_T subData;
    uint32 len = 0;

    MEMZERO(&data, sizeof(data)); 
    memcpy_s(&subData, sizeof(OOP_FROZENSETS_T), frzSets, sizeof(OOP_FROZENSETS_T));

    normal.oad.value = frzOAD.value;

    //写单元数据，先读取全量信息，没写就读会失败，所以不处理返回值
    ret = db_read_nomal(fd, &normal, sizeof(data), (uint8*)&data, &len);
    PRTL_FMT_TRACE("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
        ret, normal.logicId, normal.infoNum, normal.oad.value);

    //去除data数组中与subData的重复元素
    arrdsingle(&subData.objs, (int*)&subData.nNum, &data.objs, (int*)&data.nNum, sizeof(OOP_FROZENOBJ_T), frozen_cfg_cmp);

    //未超过最大个数才添加，否则会造成数组越界
    if((data.nNum + subData.nNum) <= OOP_MAX_FROZENOBJ)
    {
        memcpy(&data.objs[data.nNum], &subData.objs, sizeof(OOP_FROZENOBJ_T)*subData.nNum);
        data.nNum += subData.nNum;
    }
    else
    {
        PRTL_FMT_DEBUG("frozen_cfg_batch_add failed, member is full. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    //写入数据
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(fd, &normal, (uint8*)&data, sizeof(data));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return ret;
}

/**
*********************************************************************
* @name：       frozen_objattr_batch_del
* @brief：      批量删除冻结对象属性
* @param[in] ：fd           -数据中心句柄
               frzOAD      -冻结主OAD
               frzSets     -冻结关联OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_objattr_batch_del(DB_CLIENT fd, OOP_OAD_U frzOAD, OOP_FROZENSETS_T *frzSets)
{
    int ret = ERR_OK;
    NOMAL_OAD_T normal = {0};
    OOP_FROZENSETS_T data;
    uint32 len = 0;
    CLEAR_DATA_T clearData;
    uint8 i = 0, j = 0;

    MEMZERO(&clearData, sizeof(clearData));
    MEMZERO(&data, sizeof(data)); 

    normal.oad.value = frzOAD.value;

    //读取原有配置表
    ret = db_read_nomal(fd, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    //找到对应的关联属性,把位于其后的分路向前递补移动
    for(i = 0; i < frzSets->nNum; i++)
    {
        for(j = 0; j < data.nNum; j++)
        {
            if(data.objs[j].oad.value == frzSets->objs[i].oad.value)
            {
                memmove(&data.objs[j], &data.objs[j+1], (data.nNum-j-1)*sizeof(OOP_FROZENOBJ_T));
                data.nNum--;
                break;
            }
        }
    }
    
    //如果数量为0，则删除该OAD
    if(data.nNum == 0)
    {
        clearData.recordOAD.classtag = CLASS_DATA_INIT;
        clearData.recordOAD.road.oadCols.nNum = 1;
        clearData.recordOAD.road.oadCols.oad[0].value = frzOAD.value;
        
        ret = frozen_objattr_clear(fd, frzOAD);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("frozen_objattr_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
            return ret;
        }
    }
    else
    {
        //写入数据
        normal.classtag = CLASS_DATA_INIT;
        
        ret = db_write_nomal(fd, &normal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return ret;
        }
    }

    return ret;
}

/**
*********************************************************************
* @name：       frozen_tgmin15_objattr_add
* @brief：      总加组添加15分钟曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_tgmin1_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_tg50021min_objs;
    OOP_OAD_U frzOAD = {0x50020300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_tg50021min_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = tgOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 1440*7; //一天96个点，冻结7天
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_tgmin15_objattr_add
* @brief：      总加组添加15分钟曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_tgmin15_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_tg5002_objs;
    OOP_OAD_U frzOAD = {0x50020300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_tg5002_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
		#if defined (AREA_JIANGSU) && defined (PRODUCT_ZCU_1)
        if((pObjs[i].value==0x23010300)||(pObjs[i].value==0x23010400))
		{
			frzSets.objs[i].nPeriod = 1; 
	        frzSets.objs[i].nDepth = 1440*2; //一天96个点，冻结7天
		}
		else
		#endif
		{
	        
	        frzSets.objs[i].nPeriod = 15; 
	        frzSets.objs[i].nDepth = 96*7; //一天96个点，冻结7天
		}
		frzSets.objs[i].oad.nObjID = tgOAD.nObjID;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_tgday_objattr_add
* @brief：      总加组日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_tgday_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_tg5004_objs;
    OOP_OAD_U frzOAD = {0x50040300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_tg5004_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = tgOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 62;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_tgmon_objattr_add
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_tgmon_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_tg5006_objs;
    OOP_OAD_U frzOAD = {0x50060300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_tg5006_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = tgOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 31;
    }

    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_tg_objattr_add
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_tg_objattr_add(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    
    ret |= frozen_tgmin15_objattr_add(fd, tgOAD);
    ret |= frozen_tgday_objattr_add(fd, tgOAD);
    ret |= frozen_tgmon_objattr_add(fd, tgOAD);

    return ret;
}

/**
*********************************************************************
* @name：       frozen_tgmin15_objattr_del
* @brief：      总加组添加15分钟曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_tgmin15_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_tg5002_objs;
    OOP_OAD_U frzOAD = {0x50020300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_tg5002_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = tgOAD.nObjID;
        frzSets.objs[i].nPeriod = 15; 
        frzSets.objs[i].nDepth = 96*7; //一天96个点，冻结7天
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_tgday_objattr_del
* @brief：      总加组日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_tgday_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_tg5004_objs;
    OOP_OAD_U frzOAD = {0x50040300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_tg5004_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = tgOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 62;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_tgmon_objattr_del
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_tgmon_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_tg5006_objs;
    OOP_OAD_U frzOAD = {0x50060300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_tg5006_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = tgOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 31;
    }

    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_tg_objattr_add
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               tgOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_tg_objattr_del(DB_CLIENT fd, OOP_OAD_U tgOAD)
{
    int ret = ERR_OK;

    ret |= frozen_tgmin15_objattr_del(fd, tgOAD);
    ret |= frozen_tgday_objattr_del(fd, tgOAD);
    ret |= frozen_tgmon_objattr_del(fd, tgOAD);

    return ret;
}

/**
*********************************************************************
* @name：       frozen_pulsemin15_objattr_add
* @brief：      脉冲添加15分钟曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_pulsemin1_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_pulse50021min_objs;
    OOP_OAD_U frzOAD = {0x50020300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_pulse50021min_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = pulseOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 1440*7; //一天96个点，冻结7天
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_pulsemin15_objattr_add
* @brief：      脉冲添加15分钟曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_pulsemin15_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_pulse5002_objs;
    OOP_OAD_U frzOAD = {0x50020300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_pulse5002_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
		#if defined (AREA_JIANGSU) && defined (PRODUCT_ZCU_1)
		if((pObjs[i].value==0x24010500)||(pObjs[i].value==0x24010600))
		{
			frzSets.objs[i].nPeriod = 1; 
	        frzSets.objs[i].nDepth = 1440*2; //一天96个点，冻结7天
		}
		else
		#endif
		{
	        
	        frzSets.objs[i].nPeriod = 15; 
	        frzSets.objs[i].nDepth = 96*7; //一天96个点，冻结7天
		}
		frzSets.objs[i].oad.nObjID = pulseOAD.nObjID;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_pulseday_objattr_add
* @brief：      脉冲日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_pulseday_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_pulse5004_objs;
    OOP_OAD_U frzOAD = {0x50040300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_pulse5004_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = pulseOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 62;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_pulsemon_objattr_add
* @brief：      脉冲月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_pulsemon_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_pulse5006_objs;
    OOP_OAD_U frzOAD = {0x50060300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_pulse5006_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = pulseOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 31;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_pulse_objattr_add
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_pulse_objattr_add(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;

    ret |= frozen_pulsemin15_objattr_add(fd, pulseOAD);
    ret |= frozen_pulseday_objattr_add(fd, pulseOAD);
    ret |= frozen_pulsemon_objattr_add(fd, pulseOAD);

    return ret;
}

/**
*********************************************************************
* @name：       frozen_pulsemin15_objattr_del
* @brief：      脉冲添加15分钟曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_pulsemin15_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_pulse5002_objs;
    OOP_OAD_U frzOAD = {0x50020300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_pulse5002_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = pulseOAD.nObjID;
        frzSets.objs[i].nPeriod = 15; 
        frzSets.objs[i].nDepth = 96*7; //一天96个点，冻结7天
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_pulseday_objattr_del
* @brief：      脉冲日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_pulseday_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_pulse5004_objs;
    OOP_OAD_U frzOAD = {0x50040300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_pulse5004_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = pulseOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 62;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_pulsemon_objattr_del
* @brief：      脉冲月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_pulsemon_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_pulse5006_objs;
    OOP_OAD_U frzOAD = {0x50060300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_pulse5006_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].oad.nObjID = pulseOAD.nObjID;
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 31;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_pulse_objattr_add
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               pulseOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_pulse_objattr_del(DB_CLIENT fd, OOP_OAD_U pulseOAD)
{
    int ret = ERR_OK;

    ret |= frozen_pulsemin15_objattr_del(fd, pulseOAD);
    ret |= frozen_pulseday_objattr_del(fd, pulseOAD);
    ret |= frozen_pulsemon_objattr_del(fd, pulseOAD);

    return ret;
}

/**
*********************************************************************
* @name：       frozen_acmin15_objattr_add
* @brief：      总加组添加15分钟曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_acmin5_objattr_add(DB_CLIENT fd)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_ac5002_objs;
    OOP_OAD_U frzOAD = {0x50020300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_ac5002_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].nPeriod = 5; 
        frzSets.objs[i].nDepth = 288*7; //一天288个点，冻结7天
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_achour_objattr_add
* @brief：      交采小时冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_achour_objattr_add(DB_CLIENT fd)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_ac5003_objs;
    OOP_OAD_U frzOAD = {0x50030300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_ac5003_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 48;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}


/**
*********************************************************************
* @name：       frozen_acday_objattr_add
* @brief：      总加组日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_acday_objattr_add(DB_CLIENT fd)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_ac5004_objs;
    OOP_OAD_U frzOAD = {0x50040300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_ac5004_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 62;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_acmon_objattr_add
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_acmon_objattr_add(DB_CLIENT fd)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_ac5006_objs;
    OOP_OAD_U frzOAD = {0x50060300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_ac5006_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 31;
    }

    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_add(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_ac_objattr_add
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_ac_objattr_add(DB_CLIENT fd)
{
    int ret = ERR_OK;

    ret |= frozen_acmin5_objattr_add(fd);
    ret |= frozen_achour_objattr_add(fd);
    ret |= frozen_acday_objattr_add(fd);
    ret |= frozen_acmon_objattr_add(fd);

    return ret;
}

/**
*********************************************************************
* @name：       frozen_acmin15_objattr_del
* @brief：      总加组添加15分钟曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_acmin5_objattr_del(DB_CLIENT fd)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_ac5002_objs;
    OOP_OAD_U frzOAD = {0x50020300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_ac5002_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].nPeriod = 5; 
        frzSets.objs[i].nDepth = 288*7; //一天288个点，冻结7天
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_achour_objattr_del
* @brief：      交采小时冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_achour_objattr_del(DB_CLIENT fd)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_ac5003_objs;
    OOP_OAD_U frzOAD = {0x50030300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_ac5003_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 48;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_acday_objattr_del
* @brief：      总加组日曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_acday_objattr_del(DB_CLIENT fd)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_ac5004_objs;
    OOP_OAD_U frzOAD = {0x50040300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_ac5004_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 62;
    }

    //清除冻结记录
    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_acmon_objattr_del
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_acmon_objattr_del(DB_CLIENT fd)
{
    int ret = ERR_OK;
    uint8 i = 0;
    OOP_FROZENSETS_T frzSets;
    const OOP_OAD_U *pObjs = g_ac5006_objs;
    OOP_OAD_U frzOAD = {0x50060300};

    MEMZERO(&frzSets, sizeof(frzSets)); 

    frzSets.nNum = sizeof(g_ac5006_objs)/sizeof(OOP_OAD_U);

    for(i = 0; i < frzSets.nNum; i++)
    {
        frzSets.objs[i].oad.value =  pObjs[i].value; 
        frzSets.objs[i].nPeriod = 1; 
        frzSets.objs[i].nDepth = 31;
    }

    ret = frozen_data_clear(fd, frzOAD);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_data_clear failed. ret(%d), frzOAD(0x%08x)\n", ret, frzOAD.value);
    }

    return frozen_objattr_batch_del(fd, frzOAD, &frzSets);
}

/**
*********************************************************************
* @name：       frozen_ac_objattr_del
* @brief：      总加组月曲线冻结配置
* @param[in] ：fd           -数据中心句柄
               acOAD       -总加组OAD
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2020-04-09
*********************************************************************
*/
int frozen_ac_objattr_del(DB_CLIENT fd)
{
    int ret = ERR_OK;

    ret |= frozen_acmin5_objattr_del(fd);  
    ret |= frozen_achour_objattr_del(fd);
    ret |= frozen_acday_objattr_del(fd);
    ret |= frozen_acmon_objattr_del(fd);

    return ret;
}

#endif

#if DESC("冻结基础单元D2B",1)

/**
*********************************************************************
* @brief：      冻结关联对象属性表
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E data_to_buf_frozen_attr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_FROZENSETS_T data;
    uint8 offlen = 0;      //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    memcpy(&data, &srcBuf[sOffset], sizeof(data));
    sOffset += sizeof(data);

    //数组类型
    dstBuf[dOffset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(data.nNum, lenBuf);
    memcpy(&dstBuf[dOffset], lenBuf, offlen);
    dOffset += offlen;

    for(i = 0; i < data.nNum; i++)
    {
        //结构类型
        dstBuf[dOffset++] = DT_STRUCTURE;

        //结构成员数
        dstBuf[dOffset++] = 3;
                
        //冻结周期
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.objs[i].nPeriod, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nPeriod(%d)\n", i, dar, data.objs[i].nPeriod);
            return DATA_TYPE_UNMATCHED;
        }
        
        //关联对象属性
        dar = basic_data_to_buf(E_OAD, dstBuf, dstLen, &dOffset, &data.objs[i].oad, sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),oad(0x%08x)\n", i, dar, data.objs[i].oad.value);
            return DATA_TYPE_UNMATCHED;
        }

        //存储深度
        dar = basic_data_to_buf(E_LONG_UNSIGNED, dstBuf, dstLen, &dOffset, &data.objs[i].nDepth, sizeof(uint16));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d),nDepth(%d)\n", i, dar, data.objs[i].nDepth);
            return DATA_TYPE_UNMATCHED;
        }        
    }

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

#endif

#if DESC("冻结基础单元B2D",1)

/**
*********************************************************************
* @brief：      冻结关联对象属性单元
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E buf_to_data_frozen_unit(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_FROZENOBJ_T data;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;

    MEMZERO(&data, sizeof(data));
    
    //结构类型
    if(srcBuf[sOffset++] != DT_STRUCTURE)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //结构成员数
    if(srcBuf[sOffset++] != 3)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check struct num failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }  
    
    //冻结周期
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nPeriod, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //关联对象属性
    dar = basic_buf_to_data(E_OAD, srcBuf, srcLen, &sOffset, &data.oad, sizeof(OOP_OAD_U));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }
    
    //存储深度
    dar = basic_buf_to_data(E_LONG_UNSIGNED, srcBuf, srcLen, &sOffset, &data.nDepth, sizeof(uint16));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }        

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

/**
*********************************************************************
* @brief：      冻结关联对象属性表
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6                 
*********************************************************************
*/
OOP_DAR_E buf_to_data_frozen_attr(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  sOffset = *convert->sOffset;
    uint32  dOffset = *convert->dOffset;
    OOP_FROZENSETS_T data;
    uint8 offlen = 0;      //数组数量长度
    uint8 i = 0;
    uint8 *srcBuf = (uint8*)convert->srcBuf;
    uint32 srcLen = convert->srcLen;
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32 unitDoffset = 0;
    DATA_CONVERT_T unitConvert;

    MEMZERO(&data, sizeof(data));
    memcpy(&unitConvert, convert, sizeof(DATA_CONVERT_T));
    
    //数组类型
    if(srcBuf[sOffset++] != DT_ARRAY)
    {
        PRTL_BUF_DEBUG(srcBuf, srcLen, "check array type failed. sOffset(%d)", sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    //数组成员个数
    data.nNum = get_len_offset(&srcBuf[sOffset], &offlen);
    sOffset += offlen;

    *unitConvert.sOffset = sOffset;
    for(i = 0; i < data.nNum; i++)
    {
        unitDoffset = 0;
        unitConvert.dstBuf = &data.objs[i];
        unitConvert.dstLen = sizeof(OOP_FROZENOBJ_T);
        unitConvert.dOffset = &unitDoffset;

        dar = buf_to_data_frozen_unit(pOadInfo, &unitConvert);
        if(dar != DATA_SUCCESS)
        {
            PRTL_BUF_DEBUG(&srcBuf[sOffset], srcLen-sOffset, "buf_to_data_frozen_unit failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
            return dar;
        }
    }

    memcpy(&dstBuf[dOffset], &data, sizeof(data));
    dOffset += sizeof(data);

    //避免每步都判断，放最后检查，但检查前可能已经越界
    if((sOffset > srcLen) || (dOffset > dstLen))
    {
        PRTL_FMT_DEBUG("cross the border. sOffset(%d), srcLen(%d), dOffset(%d), dstLen(%d)\n", sOffset, srcLen, dOffset, dstLen);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;        
    }

    *convert->dOffset = dOffset;
    *convert->sOffset = sOffset;
     
    return dar;
}

#endif

#if DESC("数据转换总入口D2B&B2D",1)

/**
*********************************************************************
* @brief：     从数据中心的数据转698格式报文
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_class9(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_FROZENSETS_T: //冻结关联对象属性表
        {
            return data_to_buf_frozen_attr(pOadInfo, convert);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("buf_to_data_class9 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);            
        }break;        
    }
    
    return DATA_OBJECT_UNAVAILABLE;
}

/**
*********************************************************************
* @brief：      从数据中心的数据转698格式报文
* @param[in]： ppOadInfo     - 数据表指针
               srcBuf       - 源数据
               srcLen       - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen       - 目的数据长度
               sOffset      - 转换后源数据区的偏移
               dOffset      - 转换后目的数据区的偏移
* @return：     =0           - 成功
              !=0           - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_class9(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert)
{
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_FROZENSETS_T: //冻结关联对象属性表
        {
            return buf_to_data_frozen_attr(pOadInfo, convert);
        }break;

        default:
        {
            PRTL_FMT_DEBUG("buf_to_data_class9 failed. oad(0x%08x)\n", pOadInfo->pTab->oad.value);
        }break;        
    }
    
    return DATA_OBJECT_UNAVAILABLE;

}

#endif

#if DESC("SET服务数据接口",1)
/**
*********************************************************************
* @name：       frozen_objattr_set
* @brief：      设置关联对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_objattr_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_FROZENSETS_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &data;
    convert.dstLen = sizeof(data);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_frozen_attr(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_frozen_attr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    
    ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(OOP_FROZENSETS_T));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

#endif

#if DESC("GET服务数据接口",1)

/**
*********************************************************************
* @name：       frozen_objattr_get
* @brief：      获取关联对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_objattr_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_FROZENSETS_T data;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 len = 0;    

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&convert, sizeof(convert)); 

    //读出数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_FROZENSETS_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);

        //数据中心出错返回NULL
        return oop_null_array_res(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }

    //OAD与返回数据
    memcpy(&outData->pResultData[dOffset], inData, sizeof(OOP_OAD_U));
    dOffset += sizeof(OOP_OAD_U);

    outData->pResultData[dOffset++] = 1;

    //转换成报文数据
    convert.srcBuf = &data;
    convert.srcLen = sizeof(data);
    convert.sOffset = &sOffset;
    
    convert.dstBuf = outData->pResultData;
    convert.dstLen = outDataMax;
    convert.dOffset = &dOffset;
    
    dar = data_to_buf_frozen_attr(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_frozen_attr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = dOffset;

    return dar;
}

/**
*********************************************************************
* @brief：      获取冻结的所有记录列
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 frozen_all_rcsd_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax)
{
    int32 ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 offset = *pOffset;
    uint32 i = 0;
    OOP_FROZENSETS_T data;
    NOMAL_OAD_T normal = {0};
    uint32 len = 0; 
    OOP_RCSD_T  oopRcsd;   
    
    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&oopRcsd, sizeof(oopRcsd)); 

    //读出关联OAD数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.oad.attID = 3;
    
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_FROZENSETS_T), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_all_rcsd_get failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
    }

    oopRcsd.nNum = 2 + data.nNum;
    oopRcsd.cols[0].oad.value = 0x20230200;
    oopRcsd.cols[1].oad.value = 0x20210200;

    for(i = 0; i< data.nNum; i++)
    {
        oopRcsd.cols[i+2].oad.value = data.objs[i].oad.value;
    }

    if(pOadInfo->noParse)
    {
        memcpy(&pOutData[offset], &oopRcsd, sizeof(oopRcsd));
        offset += sizeof(oopRcsd);
    }
    else
    {
        dar = apdu_rcsd_tofmt698(pOutData, outDataMax, &offset, &oopRcsd, sizeof(oopRcsd));
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("apdu_rcsd_tofmt698 failed. oopRcsd.nNum(%d)\n", oopRcsd.nNum);
            return dar;        
        }
    }
    
    *pOffset = offset;
    
    return ret;
}

/**
*********************************************************************
* @brief：      获取读记录型的参数
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
* @author    : 贺宁
* @Date      ：2019-12-6               
*********************************************************************
*/
int32 frozen_record_param(RSD_T *pRSD, CSD_T *pCSD, OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, READ_RECORD_T *pInReadRecord, RELY_INFO_T *pRelyInfo, uint32 *pReadRecNum)
{
    int32 nRet = ERR_OK;
    READ_RECORD_T  inReadRecord;
    RELY_INFO_T    relyInfo;
    uint32         readRecNum = 0;   
    NOMAL_OAD_T  normal; 
    time_t tmStart = 0;
    time_t tmEnd = 0;
    time_t curTime = time(NULL);
    OOP_OCTETVAR16_T  tmnaddr = {0};        //终端通信地址    
    uint32 len = 0;
    OOP_PRSD_T oopRsd;
    uint32 i = 0;
    uint16 offset = 0;
    OAD_INFO_T oadInfo = *pOadInfo;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&relyInfo, 0, sizeof(RELY_INFO_T));
    memset(&normal, 0, sizeof(NOMAL_OAD_T));
    MEMZERO(&oopRsd, sizeof(OOP_PRSD_T));

    oopRsd.choice = pRSD->selId;

    //冻结支持方法1~3，方法9
    switch (pRSD->selId)
    {
        case 1: 
        {
            inReadRecord.sortType = UP_SORT;
             
            //方法OAD目前支持持时标类OAD
            if(pRSD->SelOad.value == 0x20210200) //数据冻结时间
            {
                inReadRecord.cType = COL_TM_STORE;
            }
            else
            {
               nRet = ERR_O_RANGE;
            }
             
            DT_DateTimeHex2Time((DateTimeHex_t *)(pRSD->StartData), &tmStart);

            // 注：+8个小时(28800)转成格林时间
            tmStart += COMPENSATION_TIME;

            inReadRecord.cStart = tmStart;
            inReadRecord.cEnd = tmStart;
            readRecNum = 0xffffffff; //传无效的记录数
        }break;
            
        case 2: 
        {
            //按时间范围筛选，采用升序？
            inReadRecord.sortType = UP_SORT;
            
            //方法OAD目前支持持时标类OAD
            if(pRSD->SelOad.value == 0x20210200) //数据冻结时间
            {
                inReadRecord.cType = COL_TM_STORE;
            }
            else
            {
               nRet = ERR_O_RANGE;
            }
            
            DT_DateTimeHex2Time((DateTimeHex_t *)(pRSD->StartData), &tmStart);

            DT_DateTimeHex2Time((DateTimeHex_t *)(pRSD->EndData), &tmEnd);

            // 注：+8个小时(28800)转成格林时间
            tmStart += COMPENSATION_TIME;
            tmEnd += COMPENSATION_TIME;
            
            inReadRecord.cStart = tmStart;
            inReadRecord.cEnd = tmEnd;
            memcpy(&inReadRecord.ti, &pRSD->Ti, sizeof(OOP_TI_T));
            readRecNum = 0xffffffff; //传无效的记录数
        }break;

        case 9: //方法9没有时标，但数据中心根据时间筛选，所以时间从0~当前时间
        {
            inReadRecord.cType = COL_TM_STORE;
            inReadRecord.sortType = DOWN_SORT;
            inReadRecord.cStart = 0;
            inReadRecord.cEnd = curTime;  
            readRecNum = pRSD->LastN;
            oopRsd.sel9.nLast = pRSD->LastN;
        }break;

        default: nRet = ERR_OPTION; break;
    }
   
    //获取RCSD信息
    if(pCSD == NULL)
    {
        PRTL_FMT_TRACE("RCSD = 0, means all record row need to response.\n");  
        oadInfo.noParse = TRUE;
        nRet = frozen_all_rcsd_get(&oadInfo, pApduInfo, &offset, (uint8*)&relyInfo.oopRcsd, sizeof(OOP_RCSD_T));
    }
    else
    {    
        /*pCSD -- > oopRcsd*/
        if (oop_rcsd_get(pCSD, &relyInfo.oopRcsd) != ERR_OK)
        {
            nRet = ERR_OPTION;
        }
    }

    if(nRet != ERR_OK)
    {
        PRTL_FMT_DEBUG("rsd,rcsd process failed.\n");
        return nRet;
    }

    //终端通信地址
    normal.oad.value = 0x40010200;
    nRet = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_OCTETVAR16_T), (uint8*)&tmnaddr, &len);
    if(nRet != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            nRet, normal.logicId, normal.infoNum, normal.oad.value);
        return nRet;
    }

    // 校验码，通信地址的前6个字节
    inReadRecord.MAC.nNum = 6;
    memcpy(inReadRecord.MAC.value, tmnaddr.value, 6);    
    
    inReadRecord.recordOAD.road.oadMain.value = pOadInfo->pTab->oad.value;
    inReadRecord.recordOAD.road.oadCols.nNum = relyInfo.oopRcsd.nNum;

    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        inReadRecord.recordOAD.road.oadCols.oad[i].value = relyInfo.oopRcsd.cols[i].oad.value;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        relyInfo.ms    = pRSD->Ms;
        relyInfo.pOadInfo = pOadInfo;
        relyInfo.oopRsd = &oopRsd;
        *pInReadRecord = inReadRecord;
        *pReadRecNum   = readRecNum;
        *pRelyInfo     = relyInfo;
    } 
        
    return nRet;
}

/**
*********************************************************************
* @name：       frozen_rectable_rec_get
* @brief：      读记录方式获取冻结记录表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_rectable_rec_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    int nRet = ERR_OK;
    uint32 offset = 0;   //输入报文buf的偏移
    uint32 csdStart = 0;
    RECORD_INFO_T recordInfo;
    BUFFER_INFO_T outRecord;
    OOP_OAD_U oopOad;
    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;   
    uint16 innerOffset = 0;
    OAD_INFO_T oadInfo;

    //参数打印
    PRTL_BUF_TRACE(inData, InDataLen, "frozen_rectable_rec_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

    //初始化
    MEMZERO(&recordInfo, sizeof(recordInfo));
    MEMZERO(&outRecord, sizeof(outRecord));
    MEMZERO(&oopOad, sizeof(oopOad));
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    memcpy(&oadInfo, pOadInfo, sizeof(OAD_INFO_T));

    //报文中提取OAD
    memcpy_r(&oopOad, &inData[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //报文中提取RSD
    dar = apdu_rsd_fromfmt698(inData, InDataLen, &offset, &oopRsd, sizeof(oopRsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RSD解析错误\n");
        return dar;
    }

    //报文中提取RCSD
    csdStart = offset;
    dar = apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_TRACE("RCSD解析错误\n");
        return dar;
    }

    if(oopRcsd.nNum == 0)
    {
        //TODO,读所有列
        oadInfo.noParse = TRUE;
        frozen_all_rcsd_get(&oadInfo, pApduInfo, &innerOffset, (uint8*)&oopRcsd, sizeof(OOP_RCSD_T));
    }

    //先封装记录响应头
    read_record_response(&oadInfo, pApduInfo, inData, InDataLen, offset, csdStart, NULL, outData, outDataMax);

    //分配读取记录的空间
    recordInfo.oopOad = oopOad;
    recordInfo.oopRsd = oopRsd;   
    recordInfo.oopRcsd = oopRcsd;
    recordInfo.append.pOadInfo = pOadInfo;
    recordInfo.append.bpInfo = pApduInfo->bpBreakInfo;
  
    outRecord.buffer = outData->pResultData;  
    outRecord.offset = &outData->resultDataLen;
    outRecord.bufLen = pApduInfo->connectionInfo.ApplSplitLen - outData->resultDataLen - cal_excludeLen(pApduInfo->connectionInfo);
    
    /*读统计记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = record_frozen_read(pApduInfo->hUdp, &recordInfo, &outRecord);
    if (nRet != 0)
    {
        PRTL_FMT_DEBUG("record_tmn_frozen_read failed. nRet(%d)\n", nRet);    
        return nRet;
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       evt_rectable_normethod_get
* @brief：      用读普通数据方式读事件记录表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_rectable_nor_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_OTHER_REASON;
    int32 nRet = ERR_OK;   
    READ_RECORD_T inReadRecord;
    RELY_INFO_T   relyInfo;
    uint32        readRecNum = 0;
    uint16 outOffset = 0;    
    uint16 headLen = 0;
    RECORD_BUF_T outRecordBuf;
    time_t curTime = time(NULL);
    OOP_OCTETVAR16_T  tmnaddr = {0};        //终端通信地址        
    NOMAL_OAD_T  normal; 
    uint32 len = 0;
    OOP_PRSD_T oopRsd;
    uint32 i = 0;
    uint16 offset = 0;
    OAD_INFO_T oadInfo;

    MEMZERO(&inReadRecord, sizeof(READ_RECORD_T));
    MEMZERO(&relyInfo, sizeof(RELY_INFO_T));    
    MEMZERO(&normal, sizeof(NOMAL_OAD_T));
    MEMZERO(&oopRsd, sizeof(OOP_PRSD_T));
    MEMZERO(&outRecordBuf, sizeof(RECORD_BUF_T));
    memcpy(&oadInfo, pOadInfo, sizeof(OAD_INFO_T));

    //参数打印
    PRTL_BUF_TRACE(inData, InDataLen, "frozen_rectable_normethod_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value + pOadInfo->nIndex);

    /*填写OAD+记录结果1 并记下偏移headLen*/
    memcpy(outData->pResultData, inData, 4);
    outOffset = 4;

    outData->pResultData[outOffset++] = 1; //[1] DATA
    
    outData->resultDataLen += outOffset;
    headLen = outOffset;

    inReadRecord.sortType = NO_SORT;
    inReadRecord.cType = COL_TM_STORE; //最近几条记录，肯定是用存储时间筛选
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = curTime+COMPENSATION_TIME+1;


    //终端通信地址  
    normal.oad.value = 0x40010200;
    nRet = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(OOP_OCTETVAR16_T), (uint8*)&tmnaddr, &len);
    if(nRet != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            nRet, normal.logicId, normal.infoNum, normal.oad.value);

        FREE_S(outRecordBuf.recordBuf);
        return DATA_READ_WRITE_DENIED;
    }
    
    // 校验码，通信地址的前6个字节,
    inReadRecord.MAC.nNum = 6;
    memcpy(inReadRecord.MAC.value, tmnaddr.value, 6);   

    //把OAD的属性索引转换成方法9，读上第n次记录
    if(pOadInfo->nIndex != 0)
    {
        oopRsd.choice = 9;
        oopRsd.sel9.nLast = pOadInfo->nIndex;
    }
    
    readRecNum = 0xffffffff; 

    relyInfo.method = 1;
    relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
    relyInfo.pOadInfo = pOadInfo;
    relyInfo.oopRsd = &oopRsd;

    //normal方式相当于筛选所有列，再添加array,struct等类型头
    oadInfo.noParse = TRUE;
    frozen_all_rcsd_get(&oadInfo, pApduInfo, &offset, (uint8*)&relyInfo.oopRcsd, sizeof(OOP_RCSD_T));
    
    //一个用于读，一个用于筛选，//TODO 很臃肿
    inReadRecord.recordOAD.road.oadMain.value = pOadInfo->pTab->oad.value;
    inReadRecord.recordOAD.road.oadCols.nNum = relyInfo.oopRcsd.nNum;

    for(i = 0; i < relyInfo.oopRcsd.nNum; i++)
    {
        inReadRecord.recordOAD.road.oadCols.oad[i].value = relyInfo.oopRcsd.cols[i].oad.value;
    }

    outRecordBuf.recordBuf = (uint8 *)malloc(pApduInfo->connectionInfo.ApplSplitLen);
    memset(outRecordBuf.recordBuf, 0xee, pApduInfo->connectionInfo.ApplSplitLen);
    outRecordBuf.tmEndSeg = 0;
    
    /*读事件记录表接口，可以读出apdu数据域，可返回断点信息*/
    if (record_tmn_frozen_read(pApduInfo->hUdp, &inReadRecord, &relyInfo, readRecNum, pApduInfo->connectionInfo.ApplSplitLen - headLen - cal_excludeLen(pApduInfo->connectionInfo), &outRecordBuf))
    {
        PRTL_FMT_DEBUG("record_event_tmn_read failed.\n");
        dar = DATA_READ_WRITE_DENIED;
    }
    else
    {
        PRTL_BUF_TRACE(outRecordBuf.recordBuf, outRecordBuf.bufLen, "record_event_tmn_read:");
        memcpy(&outData->pResultData[headLen], outRecordBuf.recordBuf, outRecordBuf.bufLen);
        outData->resultDataLen += outRecordBuf.bufLen;
    
        if (outRecordBuf.breakInfo.appSlicingFlag == 1)
        {
            outRecordBuf.breakInfo.objPre = 0x01;   //对象属性，普通数据方式读事件记录
            /*记下断点信息*/
            memcpy(pApduInfo->bpBreakInfo, &outRecordBuf.breakInfo, sizeof(BP_INFO_T));
        }
        else
        {
            /*清除断点信息*/
            memset(pApduInfo->bpBreakInfo, 0, sizeof(BP_INFO_T));
        }
        
        dar = DATA_SUCCESS;
    }

    FREE_S(outRecordBuf.recordBuf);
    
    return dar;
}

/**
*********************************************************************
* @name：       frozen_rectable_get
* @brief：      获取冻结记录表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_rectable_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    if(pOadInfo->isRec == TRUE) 
    {
        return frozen_rectable_rec_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
    }
    else
    {
        return frozen_rectable_nor_get(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);        
    }

    return dar;
}

#endif

#if DESC("ACT服务数据接口",1)

/**
*********************************************************************
* @name：       frozen_reset
* @brief：      复位
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_reset(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_OAD_U oad = {0};
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData)); 

    //构造冻结记录表的OAD
    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 2;

    //删除OAD
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = oad.value;
    
    ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return DATA_OTHER_REASON;
    }

    return dar;
}


/**
*********************************************************************
* @name：       frozen_cfg_clear
* @brief：      清除关联对象属性表
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_cfg_clear(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_OAD_U oad = {0};
    CLEAR_DATA_T clearData;

    MEMZERO(&clearData, sizeof(clearData)); 

    //构造关联对象属性表的OAD
    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    //删除OAD
    clearData.recordOAD.classtag = CLASS_DATA_INIT;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = oad.value;
    
    ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       frozen_cfg_unit_add
* @brief：      添加一个冻结对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_cfg_unit_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_FROZENSETS_T subData;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&subData, sizeof(subData)); 
    MEMZERO(&convert, sizeof(convert)); 

    subData.nNum = 1;
    
    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &subData.objs[0];
    convert.dstLen = sizeof(OOP_FROZENOBJ_T);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_frozen_unit(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_frozen_unit failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    ret = frozen_objattr_batch_add(pApduInfo->hUdp, oad, &subData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_cfg_batch_add failed. ret(%d)\n", ret);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       frozen_cfg_batch_add
* @brief：      批量添加冻结对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_cfg_batch_add(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_FROZENSETS_T subData;
    DATA_CONVERT_T convert;
    uint32 sOffset = 4;
    uint32 dOffset = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&subData, sizeof(subData)); 
    MEMZERO(&convert, sizeof(convert)); 

    convert.srcBuf = (void *)inData;
    convert.srcLen = InDataLen;
    convert.sOffset = &sOffset;

    convert.dstBuf = &subData;
    convert.dstLen = sizeof(subData);
    convert.dOffset = &dOffset;  

    //从报文中提取数据
    dar = buf_to_data_frozen_attr(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(&inData[sOffset], InDataLen-sOffset, "buf_to_data_frozen_attr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    ret = frozen_objattr_batch_add(pApduInfo->hUdp, oad, &subData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("frozen_cfg_batch_add failed. ret(%d)\n", ret);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       frozen_cfg_unit_del
* @brief：      删除一个冻结对象属性
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E frozen_cfg_unit_del(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_FROZENSETS_T data;
    OOP_OAD_U delID;
    CLEAR_DATA_T clearData;
    uint32 sOffset = 4;
    uint32 len = 0;
    uint32 i = 0;
    OOP_OAD_U oad = {0};

    MEMZERO(&data, sizeof(data)); 
    MEMZERO(&clearData, sizeof(clearData));     
    MEMZERO(&delID, sizeof(delID)); 

    oad.nObjID = pOadInfo->pTab->oad.nObjID;
    oad.attID = 3;

    //从报文中提取数据
    dar = basic_buf_to_data(E_OAD, inData, InDataLen, &sOffset, &delID, sizeof(delID));
    if(dar != DATA_SUCCESS)
    {
        PRTL_BUF_DEBUG(inData, InDataLen, "basic_data_to_buf failed. dar(%d), sOffset(%d)\n", dar, sOffset);
        return DATA_TYPE_UNMATCHED;
    }

    normal.oad.value = oad.value;

    //读取原有配置表,读不到认为已经删了,直接返回成功
    ret = db_read_nomal(pApduInfo->hUdp, &normal, sizeof(data), (uint8*)&data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_SUCCESS;
    }

    //找到对应的关联属性,把位于其后的分路向前递补移动
    for(i = 0; i < data.nNum; i++)
    {
        if(data.objs[i].oad.value == delID.value)
        {
            memmove(&data.objs[i], &data.objs[i+1], (data.nNum-i-1)*sizeof(OOP_FROZENOBJ_T));
            data.nNum--;
            break;
        }
    }

    //如果数量为0，则删除该OAD
    if(data.nNum == 0)
    {
        clearData.recordOAD.classtag = CLASS_DATA_INIT;
        clearData.recordOAD.road.oadCols.nNum = 1;
        clearData.recordOAD.road.oadCols.oad[0].value = oad.value;
        
        ret = db_clear_oad_data(pApduInfo->hUdp, &clearData);
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_clear_oad_data failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
            return DATA_OTHER_REASON;
        }
    }
    else
    {
        //写入数据
        normal.classtag = CLASS_DATA_INIT;
        
        ret = db_write_nomal(pApduInfo->hUdp, &normal, (uint8*)&data, sizeof(data));
        if(ret != ERR_OK)
        {
            PRTL_FMT_DEBUG("db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                ret, normal.logicId, normal.infoNum, normal.oad.value);
            return DATA_OTHER_REASON;
        }
    }
    
    return dar;
}


#endif

#if DESC("class9对外接口函数",1)

/**
*********************************************************************
* @name：       class9_invoke_set
* @brief：      class9的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class9_invoke_set(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 3:   //关联对象属性表 //TODO 参数变化，要清空冻结数据？
        {
            dar = frozen_objattr_set(pOadInfo,  pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}

/**
*********************************************************************
* @name：       class9_invoke_get
* @brief：      class9的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class9_invoke_get(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 2:   //冻结记录表
        {
            dar = frozen_rectable_get(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        case 3:   //关联对象属性表
        {
            dar = frozen_objattr_get(pOadInfo, pApduInfo, inData, InDataLen, outData, outDataMax);
        }break;   
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;
}

/**
*********************************************************************
* @name：       class9_invoke_act
* @brief：      class9的ACTION服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：OOP_DAR_E
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class9_invoke_act(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint32 inLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 1:   //复位
        {
            dar = frozen_reset(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break; 
        
        case 4:   //添加一个冻结对象属性
        {
            dar = frozen_cfg_unit_add(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   
        
        case 5:   //删除一个冻结对象属性
        {
            dar = frozen_cfg_unit_del(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   

        case 7:   //批量添加冻结对象属性
        {
            dar = frozen_cfg_batch_add(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   

        case 8:   //清除关联对象属性表
        {
            dar = frozen_cfg_clear(pOadInfo, pApduInfo, inData, (uint16)inLen, outData, outDataMax);
        }break;   
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    return dar;    
}


#endif
