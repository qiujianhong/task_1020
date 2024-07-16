/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief： olog
* @date： 2020-6-10
*********************************************************************
*/
#include "olog.h"

/**
*********************************************************************
* @brief：olog初始化
* @param[in]： confpath 配置路径
* @param[out]：none
* @return：none
*********************************************************************
*/
int  olog_init(const char *confpath)
{
#if defined ENABLE_OLOG
    return zlog_init(confpath);
#else
    return 0;
#endif
}

/**
*********************************************************************
* @brief：olog重载
* @param[in]： confpath 配置路径
* @param[out]：tv
* @return：none
*********************************************************************
*/
int  olog_reload(const char *confpath)
{
#if defined ENABLE_OLOG
    return zlog_reload(confpath);
#else
    return 0;
#endif

}

/**
*********************************************************************
* @brief：olog关闭
* @param[in]： NONE
* @param[out]：NONE
* @return：none
*********************************************************************
*/
void olog_fini(void)
{
#if defined ENABLE_OLOG
    return zlog_fini();
#else
    return 0;
#endif

}

/**
*********************************************************************
* @brief：获取策略
* @param[in]： cname 配置文件名
* @param[out]：none
* @return：none
*********************************************************************
*/
void *olog_get_category(const char *cname)
{
#if defined ENABLE_OLOG
    return zlog_get_category(cname);
#else
    return 0;
#endif
}

/**
*********************************************************************
* @brief：dolog初始化
* @param[in]： confpath 配置文件名
* @param[out]：none
* @return：none
*********************************************************************
*/
int dolog_init(const char *confpath, const char *cname)
{
#if defined ENABLE_OLOG
    return dzlog_init(confpath, cname);
#else
    return 0;
#endif

}