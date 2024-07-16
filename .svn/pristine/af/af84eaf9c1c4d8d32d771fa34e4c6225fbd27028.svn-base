//测量点有效表

#ifndef MTRCONFIG_H
#define MTRCONFIG_H

//通道0对应的测量点信息文件
#define MTRCONFIG0 "MTRCONFIG0"
//通道1对应的测量点信息文件
#define MTRCONFIG1 "MTRCONFIG1"
//通道2对应的测量点信息文件
#define MTRCONFIG2 "MTRCONFIG2"
//通道3对应的测量点信息文件
#define MTRCONFIG3 "MTRCONFIG3"
//通道4对应的测量点信息文件
#define MTRCONFIG4 "MTRCONFIG4"
//通道5对应的测量点信息文件
#define MTRCONFIG5 "MTRCONFIG5"
//通道6对应的测量点信息文件
#define MTRCONFIG6 "MTRCONFIG6"

//通道31对应的测量点信息文件  //载波测量点
#define MTRCONFIG31 "MTRCONFIG31"

//个数长度
#define MTRVALID_COUNT_LEN     2
//个数偏移量
#define MTRVALID_COUNT_OFFSET  0

//抄表执行到的项长度
#define MTRPERFORM_INDEX_LEN 2
//抄表执行到的项偏移量
#define MTRPERFORM_INDEX_OFFSET (MTRVALID_COUNT_LEN+MTRVALID_COUNT_OFFSET)

//抄表标志长度        0x55开始抄表 0xAA结束抄表 其它无效
#define MTRPERFORM_TAG_LEN 1
//抄表标志偏移量
#define MTRPERFORM_TAG_OFFSET (MTRPERFORM_INDEX_LEN+MTRPERFORM_INDEX_OFFSET)

//测量点号长度
#define MTRINDEX_NO_LEN 2
//测量点号偏移量
#define MTRINDEX_NO_OFFSET(a) (MTRPERFORM_TAG_LEN+MTRPERFORM_TAG_OFFSET+(a)*2)

#endif


