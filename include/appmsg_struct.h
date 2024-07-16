/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   系统APP和基础APP交互消息结构体定义
* @date：    2020/1/3
* @history： 
*********************************************************************
*/
#ifndef __APPMSG_STRUCT_H__
#define __APPMSG_STRUCT_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    uint8    year_l;
    uint8    year_h;
    uint8    month;
    uint8    day;
    uint8    hour;
    uint8    min;
    uint8    sec;
}__attribute__ ((packed))Date_Time_S;

typedef struct
{
    uint16   year;
    uint8    month;
    uint8    day;
    uint8    week; //0表示周日，1…6分别表示周一到周六
    uint8    hour;
    uint8    min;
    uint8    sec;
    uint16   milsec;
    uint8    res[2];
}Date_Time;

/* 变长 */
typedef struct tagAPPMSGString
{
    uint32  num; 
    char    value[64];
}APPMSG_STRING_T;

/* 变长 */
typedef struct tagAPPMSG256String
{
    uint32  num; 
    char    value[256];
}APPMSG_256_STRING_T;

/* 执行维护命令 */
typedef struct tagAPPCOMMCMD
{
    uint8     cmdNum;  //命令行个数
    uint8     res[3];
    uint8   **ppCmd;
}APPMSG_COMMON_SYSCMD_T;


#define APPMSG_NAME_LEN     128
typedef struct tagMqtPlugin
{
    uint8       comName[APPMSG_NAME_LEN]; //组件名称
    uint32      subscribeInfo;            //订阅事件
    char        version[64];              //版本信息
    Date_Time_S dataTime;                 //发布日期
}MQT_PLUGIN_T;

/* App请求注册 */
typedef struct tagAPPSYSRegister
{
    int          pid;                    //进程号
    MQT_PLUGIN_T tPlugin;
}APPMSG_APP_REGISTER_T;

/* App名称解析 */
typedef struct tagAPPInfo
{
    char         name[APPMSG_NAME_LEN];
    uint32       ipAddr;
    uint16       port;
    char         commType;  /* 通信类型, T U M I */
    uint8        Online;
    uint32       version;  //版本号
    Date_Time_S  time;     //发布时间
}APP_INFO_T;

typedef struct sucTime
{
    uint16            ConnectNum;  /* 当日连接次数 */
    uint16            FailedNum;   /* 当日失败次数 */
    uint32            SuccessTimes;/* 连接成功时间s */
}WIRELESS_TIME_T;    /*时间结构*/

typedef struct tagCommMygmr
{
    uint8  manuNum[4];      /* 厂商代码 */
    uint8  manuExten[8];    /* 模块型号 */
    uint8  softversion[4];  /* 软件版本号 */
    uint8  softdata[6];     /* 软件版本日期 */
    uint8  hwversion[4];    /* 硬件版本号 */
    uint8  hwdata[6];       /* 硬件版本日期 */
}__attribute__ ((packed))COMM_MYGMR_T; /* 版本信息 */

/* 无线模块模块信息 */
typedef struct tagWireInfo
{
    uint8                instatus; //插入状态
    COMM_MYGMR_T         version;  //版本信息
    APPMSG_256_STRING_T  typeinfo; //模块类型信息
    APPMSG_STRING_T      IMEI;     //模块识别码IMEI
}WIRELESS_INFO_T;

/* 无线模块拨号状态 */
typedef struct tagWireDiagInfo
{
    uint8            simstatus;      //SIM卡状态
    uint8            diagstatus;     //拨号状态
    APPMSG_STRING_T  CCID;
    APPMSG_STRING_T  IMSI;
    APPMSG_STRING_T  phonenum;       //手机号
    uint8            csqvalue;       //信号强度
    uint8            operat;         //运营商
    uint8            networkType;    //网络制式
    uint8            IP[4];
    uint8            MASK[4];
    uint8            GETWAY[4];
    OOP_VISIBLEVAR_T APN;            //拨号所选APN
    uint8            diagtype;       //拨号类型: 0-自动切换协议栈;1-内置协议栈;2-外置协议栈
}WIRE_DIAG_INFO_T;

/* 发送短信 */
typedef struct tagWireSendSMS
{
    uint8       NO;           //设备号
    uint8       res[3];
    uint8       phonenum[32]; //手机号码
    uint16      len;          //信息长度
    uint8       data[0];      //内容
}WIRE_SEND_SMS_T;

/* 接收短信 */
typedef struct tagWireRecvSMS
{
    uint8       phonenum[32]; //手机号码
    uint16      len;          //信息长度
    uint8       data[0];      //内容
}WIRE_RECV_SMS_T;

/* 无线APN */
typedef struct tagWireAPN
{
    uint8               operat;  //运营商 中国移动(0), 中国联通(1), 中国电信(2), 专网(3)， 未知(4, 255)
    uint8               res[3];
    OOP_VISIBLEVAR_T    apn;     //APN
    OOP_VISIBLEVAR_T    username;//用户名
    OOP_VISIBLEVAR_T    pwd;     //密码
}WIRE_APN_T;

#define WIRELESS_DIAGAPN_MAX  8
/* 指定APN拨号 */
typedef struct tagWireDiagAPN
{
    uint8           id;         //设备号
    uint8           stack;      //拨号类型 自动切换协议栈(0), 内置协议栈(1), 外置协议栈(2)
    uint8           network;    //锁定网络0-自动，1-2G ... 4-5G
    uint8           authtype;   //0-NULL, 1-chap, 2-pap, 3-pap&chap, 255-自适应
    uint8           diagnum;    //参数数量
    uint8           res[3];
    WIRE_APN_T      tDiagApn[WIRELESS_DIAGAPN_MAX];  //运营商参数
}WIRE_DIAG_APN_T;

/* 查询模组信息 */
typedef struct tagModChannel
{
    uint8            type;      //接口类型
    uint8            function;  //功能配置
    uint8            res[2];
    APPMSG_STRING_T  dev;
}MOD_CHANNEL_T;

#define MOD_CHANNEL_MAX    8
/* 查询模组信息 */
typedef struct tagModInfoAck
{
    uint8            port;         //槽位号
    uint8            subport;      //子模块号
    uint8            status;       //状态 0-模组已识别;1-模组未识别;2-模组未接入
    uint8            num;          //通道数量

    APPMSG_STRING_T  type;         //模块设备型号
    APPMSG_STRING_T  Id;           //模块设备ID
    int              softVersion;  //软件版本
    APPMSG_STRING_T  softDate;     //软件发布日期
    int              hwVersion;    //硬件版本
    APPMSG_STRING_T  hwDate;       //硬件发布日期
    APPMSG_STRING_T  manuCode;     //厂商编码

    MOD_CHANNEL_T    channel[MOD_CHANNEL_MAX];   //通道配置
}MOD_INFO_ACK_T;

/* 查询模组信息 增加子模组信息 */
#define SUB_MOD_MAX    4
typedef struct tagModsInfoAck
{
    uint8            bSubMod;  /* 是否是扩展模块 */
    uint8            num;
    uint8            res[2];
    MOD_INFO_ACK_T   info[SUB_MOD_MAX];
}MODS_INFO_ACK_T;

/* 查询模组信息 增加子模组信息 新版本 */
typedef struct tagModInfoNew
{
    uint8            port;         //槽位号
    uint8            subport;      //子模块号
    uint8            status;       //状态 0-模组已识别;1-模组未识别;2-模组未接入
    uint8            num;          //通道数量

    APPMSG_STRING_T  type;         //模块设备型号
    APPMSG_STRING_T  Id;           //模块设备ID
    APPMSG_STRING_T  softVersion;  //软件版本
    APPMSG_STRING_T  softDate;     //软件发布日期
    APPMSG_STRING_T  hwVersion;    //硬件版本
    APPMSG_STRING_T  hwDate;       //硬件发布日期
    APPMSG_STRING_T  manuCode;     //厂商编码

    MOD_CHANNEL_T    channel[MOD_CHANNEL_MAX];   //通道配置
}MOD_INFO_NEW_T;

typedef struct tagModsInfoNew
{
    uint8            bSubMod;  /* 是否是扩展模块 */
    uint8            num;
    uint8            res[2];
    MOD_INFO_NEW_T   info[SUB_MOD_MAX];
}MODS_INFO_NEW_T;

/** @brief 地理位置信息            */
typedef struct tagGPSPOSITION
{
    uint8        position;        /**< 方位     经度:{0：E,1：W}   纬度:{0：S,1：N}    */
    uint16        degre;          /**< 度    */
    float32        branch;        /**< 分    */
}GPS_POSITION;

typedef struct tagGPSPosition
{
    GPS_POSITION    longitude;       /**< 经度    */
    GPS_POSITION    latitude;        /**< 纬度    */
    int32           height ;         /**< 高度    */
}GPS_POSTISION_T;

/* GPS定位信息 */
typedef struct tagGPS
{
    OOP_DATETIME_T  utctime;  /* UTC时间 */
    GPS_POSTISION_T position;
    uint8           ststus;   /* 定位状态(按照新版本) 旧: 0-有效定位, 1-无效定位; 新: 0-未定位,1-有效定位,2-无效定位,3-正在估算 */
    uint8           mode;     /* 模式指示 自主定位(0), 差分(1), 估算(2), 数据无效(4) */
    uint8           system;   /* 0-其他, 1-GPS, 2-BD, 3-混合 */
    uint8           res[1];
}GPS_T;

/* GPS定位消息 */
typedef struct tagGPSMSG
{
    OOP_DATETIME_T  utctime;  /* UTC时间 */
    OOP_GEOGADD_T   position;
    uint8           ststus;   /* 定位状态 有效定位（0），无效定位（1） */
    uint8           mode;     /* 模式指示 自主定位(0), 差分(1), 估算(2), 数据无效(4) */
    uint8           res[2];
}GPS_MSG_T;

#define RSP_RMT_MAX      16
#define RSP_RMT_HIS_MAX  3

/* 遥信信息 */
typedef struct tagRspRmt
{
    uint8     num; /* 数量 */
    uint16    ST;  /* 遥信状态 */
    uint16    CT;  /* 遥信变位状态 */
    uint8     flag[RSP_RMT_MAX];
    Date_Time time[RSP_RMT_MAX];
}RST_RMT_T;

typedef struct tagRspRmtAll
{
    uint8     num; /* 数量 */
    uint8     ST;  /* 遥信状态 */
    uint8     hisNum;
    RST_RMT_T history[RSP_RMT_HIS_MAX];
}RST_RMT_ALL_T;


#define RSP_PULSE_MAX  16
/* 脉冲信息 */
typedef struct tagRspPulse
{
    uint32    count;  //脉冲个数
    uint32    timesec;//统计时间s
    uint16    timems; //统计时间ms
    uint8     flag;   //有效标记
    uint8     res[1];
    Date_Time start;  //启动时间
}RST_PULSE_T;
/* 脉冲信息 */
typedef struct tagRspPulseStatic
{
    uint8       num; /* 数量 */
    uint8       res[3];
    RST_PULSE_T item[RSP_PULSE_MAX];
}RST_PULSE_STATIC_T;

/* system接口*/

/* APP注册信息 */
#define SYS_APPLIST_MAX    24
typedef struct tagSysAppList
{
    uint32                  num;
    APPMSG_STRING_T         name[SYS_APPLIST_MAX];/* 应用名称 */
}SYS_APPLIST_T;

/* APP注册信息 */
typedef struct tagSysAppReg
{
    APPMSG_STRING_T         name;               /* 应用名称 */
    uint32                  version;            /* 版本信息 */
    Date_Time               releaseDate;        /* 发布日期，格式示例："2018-03-28" */
}SYS_APPREG_T;

/* 设备信息 */
typedef struct tagSysDevInfo
{
    APPMSG_256_STRING_T     vendorId;            /* 厂商 ID */
    APPMSG_STRING_T         devSn;               /* 终端序列号 */
    APPMSG_STRING_T         ESN;
    APPMSG_STRING_T         osName;              /* 操作系统名称 */
    APPMSG_STRING_T         osVer;               /* 操作系统版本号 */
    APPMSG_STRING_T         osPatchVer;          /* 操作系统补丁版本号 */
    APPMSG_STRING_T         osKernelVer;         /* 操作系统内核版本号 */
    APPMSG_STRING_T         hardwareModel;       /* 终端硬件型号 */
    APPMSG_STRING_T         hardwareVersion;     /* 终端硬件版本 */
    APPMSG_STRING_T         manufDate;           /* 终端生产日期 */
    uint32                  cpuFreq;             /* CPU主频，单位MHZ */
    uint32                  cpuCoreNumber;       /* CPU核数 */
    APPMSG_STRING_T         cpuArch;             /* CPU架构 */
    uint32                  cpuCache;            /* CPU缓存，以MByte为单位 */
    uint32                  diskSize;            /* 硬盘大小，以MByte为单位 */
    uint32                  memSize;             /* 内存大小，以MByte为单位 */
    uint32                  virtMemSize;         /* 虚拟内存大小，以MByte为单位 */
}SYS_DEVINFO_T;

/* 设备状态 */
typedef struct tagSysDevStatus
{
    float                   cpuUsage;            /* 终端CPU使用率，百分比数据 */
    float                   memUsage;            /* 终端内存使用率，百分比数据 */
    float                   cpuTemperature;      /* 终端CPU温度 */

    float                   allDisk;             /* 总体磁盘使用率，百分比数据 */
    float                   sysDisk;             /* 系统分区磁盘使用率，百分比数据 */
    float                   containerDisk;       /* 容器分区磁盘使用率，百分比数据 */
    float                   dataDisk;            /* 数据分区磁盘使用率，百分比数据 */
    
    Date_Time               devSartTime;         /* 终端启动时间。遵循ISO8601标准的日期和时间组合表示法 */
    uint32                  devRunTime;          /* 终端运行时长，以秒为单位 */
}SYS_DEVSTATUS_T;

/* 以太网络配置 */
typedef struct tagSysEthCfg
{
    char                    ifname[24];          /* 以太网接口名称 */
    uint8                   mode;                /* IP地址配置模式: 0-DHCP动态分配; 1-静态设置 */
    uint8                   res[3];

    uint8                   ip[4];
    uint8                   netMask[4];
    uint8                   gateway[4];
    uint8                   mac[6];
}SYS_ETHCFG_T;

/* 应用信息 */
typedef struct tagSysAppInfo
{
    APPMSG_STRING_T         appName;            /* APP名称 */
    APPMSG_STRING_T         container;          /* APP所在容器的名称 */
    APPMSG_STRING_T         appVer;             /* APP版本信息 */
}SYS_APPINFO_T;

/* 容器信息列表 */
#define SYS_CONATINER_MAX    16

typedef struct tagSysContainer
{
    APPMSG_STRING_T         id;                /* 容器id */
    APPMSG_STRING_T         name;              /* 容器名 */
    APPMSG_STRING_T         status;            /* 容器状态 */
    uint32                  cpu_num;           /* 容器分配的cpu核数 */
    APPMSG_STRING_T         mem_limit;         /* 内存限值 */
    APPMSG_STRING_T         disk_limit;        /* 磁盘限值 */
    APPMSG_256_STRING_T     mount_dir;         /* 挂载目录 */
    APPMSG_STRING_T         dev;               /* 映射设备 */
    float                   cpu_usage;         /* cpu使用率，百分比数据 */
    float                   mem_usage;         /* 内存使用率，百分比数据 */
    float                   disk_usage;        /* 磁盘使用率，百分比数据 */
    uint8                   ip[4];             /* 容器IP地址 */
    APPMSG_STRING_T         img_name;          /* 容器镜像名称 */
    Date_Time               create_time;       /* 容器创建时间 */
    Date_Time               start_time;        /* 容器启动时间 */
    uint32                  run_time;          /* 容器运行时长，以秒为单位 */
    APPMSG_STRING_T         app_path;          /* APP文件存放路径 */
}SYS_CONTAINER_T;

typedef struct tagSysContainerList
{
    uint32                  num;
    SYS_CONTAINER_T         list[SYS_CONATINER_MAX];
}SYS_CONTAINER_LIST_T;

/* 容器内APP信息列表 */
#define SYS_CONATINERAPP_MAX    16

typedef struct tagSysContainerAPP
{
    uint32                  appId;              /* APP序号 */
    APPMSG_STRING_T         appName;            /* APP名称 */
    APPMSG_STRING_T         container;          /* APP所在容器名称 */
    APPMSG_STRING_T         appVer;             /* APP版本信息 */
    APPMSG_STRING_T         appHash;            /* APP哈希值 */
    uint32                  isEnable;           /* 使能状态: 0:未使能,1:使能  */
    uint32                  appStatus;          /* 运行状态: 0:停止,1:运行 */
    float                   appCpuUsageLmt;     /* APPCPU使用率限值 */
    float                   appCpuUsage;        /* APPCPU使用率 */
    float                   appMemUsage;        /* APP内存使用率 */
    Date_Time               appStartTime;       /* 容器启动时间 */
}SYS_CONTAINER_APP_T;

typedef struct tagSysContainerAPPList
{
    uint32                  num;
    SYS_CONTAINER_APP_T     list[SYS_CONATINERAPP_MAX];
}SYS_CONTAPP_LIST_T;

/* APPID */
typedef struct tagSysAppID
{
    APPMSG_STRING_T         appName;            /* APP名称 */
    APPMSG_STRING_T         appId;              /* APP_ID信息 */
}SYS_APPID_T;

/* APPID */
typedef struct tagSysAppSID
{
    uint32                  num;                  /* 数量 */
    SYS_APPID_T             app[SYS_APPLIST_MAX]; /* APPID */
}SYS_APPSID_T;

/* license */
typedef struct tagSysAppLic
{
    APPMSG_STRING_T         appName;            /* APP名称 */
    APPMSG_STRING_T         appNo;              /* APP编号 HEX格式 */
    APPMSG_STRING_T         appLic;             /* APP license信息 HEX格式 */
}SYS_APPLIC_T;

/* license */
#define SYS_APP_LIC_MAX   32
typedef struct tagSysAppSLic
{
    uint32                  appnum;             /* APP数量 */
    SYS_APPLIC_T            list[SYS_APP_LIC_MAX];
}SYS_APPS_LIC_T;

/* 路由 */
typedef struct tagSysRoute
{
    uint8                   destIp[4];          /* 目标网络地址 */
    uint8                   netMask[4];         /* 子网掩码 */
    uint8                   gateway[4];         /* 网关地址 */
    APPMSG_STRING_T         ifname;             /* 接口名称 */
}SYS_ROUTE_T;

/* SNTP对时配置 */
typedef struct tagSysSntp
{
    uint8                   isEnable;           /* SNTP使能状态: 0-未使能; 1-已使能 */
    uint8                   res[3];
    uint8                   sntpAddr[4];        /* SNTP服务器IP地址 */
    uint32                  interval;           /* 对时时间间隔，以秒为单位 */
}SYS_SNTP_T;

typedef struct tagSysInstApp
{
    APPMSG_STRING_T         conname;           /* 容器名 */
    APPMSG_STRING_T         name;              /* APP名 */
    uint8                   bCpu;
    uint8                   bMem;
    uint8                   res[2];
    uint32                  cpu_num;           /* 容器分配的cpu核数 */
    uint32                  cpu_limit;         /* cpu使用率限制，百分比数据 */
    uint32                  memory;            /* 内存限值，M Byte */
    uint32                  mem_limit;         /* 监控阈值，百分比数据  */
}SYS_INSTALL_APP_T;

/* 容器配置 */
typedef struct tagSysInstContainer
{
    APPMSG_STRING_T         name;              /* 容器名 */
    uint8                   bCpu;
    uint8                   bMem;
    uint8                   bDisk;
    uint8                   res[1];
    uint32                  cpu_num;           /* 容器分配的cpu核数 */
    uint32                  cpu_limit;         /* cpu使用率限制，百分比数据 */
    uint32                  memory;            /* 内存限值，M Byte */
    uint32                  mem_limit;         /* 监控阈值，百分比数据  */
    uint32                  disk;              /* 磁盘限值，M Byte  */
    uint32                  disk_limit;        /* 监控阈值，百分比数据  */

    uint8                   port_num;          /* 容器端口数量 */
    uint8                   dev_num;           /* 设备数量数量 */
    uint8                   mount_num;         /* 文件数量 */
    uint8                   withapp;           /* APP安装 */
    APPMSG_STRING_T         port[2];           /* 容器端口数量 */
    APPMSG_STRING_T         dev[8];            /* 映射的本地文件目录资源配置参数 */
    APPMSG_STRING_T         mount[4];          /* 映射的本地物理接口资源配置参数 */
    SYS_INSTALL_APP_T       app;               /* APP信息 */
}SYS_INSTALL_CON_T;


/* 对时事件 */
typedef struct tagSysTimeEvent
{
    Date_Time               oldTime;            /* 对时前时间 */
    Date_Time               newTime;            /* 对时时间 */
}SYS_TIME_E_T;

typedef struct _RADIO_PARAM_T
{
    uint8    channel; //频道
    uint8    power;//功率
    uint8    SquelchLecel;//静噪级别
    OOP_OCTETVAR16_T    DistrictCode;//行政区号码
    OOP_OCTETVAR16_T    devaddr;//设备地址
    OOP_OCTETVAR16_T    terminaladdr;//终端地址
}RADIO_PARAM_T;

/* 电台状态 */
typedef struct tagRADIO_STATUS
{   uint8         network;   //网络
    uint16        field;     //场强
    uint8         speaker;   //扬声器
}RADIO_STATUS_T;

#ifdef __cplusplus
}
#endif

#endif /* __APPMSG_STRUCT_H__ */
