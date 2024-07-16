/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   日志记录头文件
* @date：    2020-4-21
* @history： 
*********************************************************************
*/
#ifndef __COMM_LOG_H__
#define __COMM_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DESC(desc,v) (v)


#define MAX_THREAD  		12
#define SEND_PARAM_NUM 		2000
#define SEND_NORMAL_NUM 	2000
#define SEND_RECORD_NUM 	2000
#define SEND_RAM_NUM 	    5000
#define SEND_BIGDATA_NUM 	500

#define MAX_MSG_BUF_LEN          8192


#define MAX_SEND_NUM 		10
#define SEND_BUF_LEN  		8
#define KB 1024.0       // 2^10
#define MB 1048576.0    // 2^20 
#define GB 1073741824.0 // 2^30 

#define DPRINT(x...) ({ if (getenv("DEBUG_DBFUNC") != NULL)  printf(x);})
  
#define DPRINT_MSG(b, l, d) \
        do{\
            if(NULL != getenv("DEBUG_DBFUNC"))\
            {\
                printf(d);\
                int iii = 0;\
                for(; iii < l; iii++)\
                {\
                    printf("%02x ", b[iii]);\
                }\
                printf("\n");\
            }\
        }while(0);



#ifdef __cplusplus
}
#endif

#endif //__COMM_LOG_H__

