#ifndef __O_LOG_H__
#define __O_LOG_H__


#include "commonConfig.h"
#include "zlog.h"

#ifdef __cplusplus
extern "C" {
#endif

int  olog_init(const char *confpath);
int  olog_reload(const char *confpath);
void olog_fini(void);
void *olog_get_category(const char *cname);
int  dolog_init(const char *confpath, const char *cname);


#if defined ENABLE_OLOG

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
/* zlog macros */
#define olog_fatal(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, __VA_ARGS__)
#define olog_error(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, __VA_ARGS__)
#define olog_warn(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, __VA_ARGS__)

#ifdef ENABLE_INFO_PRINT
#define olog_notice(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, __VA_ARGS__)
#define olog_info(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, __VA_ARGS__)
#define olog_debug(cat, ...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define olog_notice(cat, ...)
#define olog_info(cat, ...)
#define olog_debug(cat, ...)
#endif

/* dzlog macros */
#define dolog_fatal(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, __VA_ARGS__)
#define dolog_error(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, __VA_ARGS__)
#define dolog_warn(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, __VA_ARGS__)

#ifdef ENABLE_INFO_PRINT
#define dolog_notice(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, __VA_ARGS__)
#define dolog_info(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, __VA_ARGS__)
#define dolog_debug(...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define dolog_notice(...)
#define dolog_info(...)
#define dolog_debug(...)
#endif

#elif defined __GNUC__
/* zlog macros */
#define olog_fatal(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, ##args)
#define olog_error(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, ##args)
#define olog_warn(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, ##args)

#ifdef ENABLE_INFO_PRINT
#define olog_notice(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, ##args)
#define olog_info(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, ##args)
#define olog_debug(cat, format, args...) \
	zlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, ##args)
#else
#define olog_notice(cat, ...)
#define olog_info(cat, ...)
#define olog_debug(cat, ...)
#endif

/* dzlog macros */
#define dolog_fatal(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, ##args)
#define dolog_error(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, ##args)
#define dolog_warn(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, ##args)

#ifdef ENABLE_INFO_PRINT
#define dolog_notice(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, ##args)
#define dolog_info(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, ##args)
#define dolog_debug(format, args...) \
	dzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, ##args)
#else
#define dolog_notice(...)
#define dolog_info(...)
#define dolog_debug(...)
#endif
#endif



/* hzlog macros */
#define holog_fatal(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, buf, buf_len)
#define holog_error(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, buf, buf_len)
#define holog_warn(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, buf, buf_len)

#ifdef ENABLE_INFO_PRINT
#define holog_notice(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, buf, buf_len)
#define holog_info(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, buf, buf_len)
#define holog_debug(cat, buf, buf_len) \
	hzlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, buf, buf_len)
#else
#define holog_notice(cat, ...)
#define holog_info(cat, ...)
#define holog_debug(cat, ...)
#endif

/* hdzlog macros */
#define hdolog_fatal(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, buf, buf_len)
#define hdolog_error(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, buf, buf_len)
#define hdolog_warn(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, buf, buf_len)
#define hdolog_notice(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, buf, buf_len)
#define hdolog_info(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, buf, buf_len)
#define hdolog_debug(buf, buf_len) \
	hdzlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, buf, buf_len)


#else


#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
/* zlog macros */
#define olog_fatal(cat, ...)  
#define olog_error(cat, ...)  
#define olog_warn(cat, ...) 
#define olog_notice(cat, ...)  
#define olog_info(cat, ...)  
#define olog_debug(cat, ...)  
/* dzlog macros */
#define dolog_fatal(...)  
#define dolog_error(...)  
#define dolog_warn(...)  
#define dolog_notice(...)  
#define dolog_info(...)  
#define dolog_debug(...)  
#elif defined __GNUC__
/* zlog macros */
#define olog_fatal(cat, format, args...)  
#define olog_error(cat, format, args...)  
#define olog_warn(cat, format, args...)  
#define olog_notice(cat, format, args...) 
#define olog_info(cat, format, args...)  
#define olog_debug(cat, format, args...) 
/* dzlog macros */
#define dolog_fatal(format, args...)  
#define dolog_error(format, args...)  
#define dolog_warn(format, args...)  
#define dolog_notice(format, args...) 
#define dolog_info(format, args...)  
#define dolog_debug(format, args...)  
#endif


/* hzlog macros */
#define holog_fatal(cat, buf, buf_len)  
#define holog_error(cat, buf, buf_len)  
#define holog_warn(cat, buf, buf_len)  
#define holog_notice(cat, buf, buf_len) 
#define holog_info(cat, buf, buf_len)  
#define holog_debug(cat, buf, buf_len) 


/* hdzlog macros */
#define hdolog_fatal(buf, buf_len)  
#define hdolog_error(buf, buf_len)  
#define hdolog_warn(buf, buf_len)  
#define hdolog_notice(buf, buf_len) 
#define hdolog_info(buf, buf_len)  
#define hdolog_debug(buf, buf_len) 

#endif

#ifdef __cplusplus
}
#endif

#endif
