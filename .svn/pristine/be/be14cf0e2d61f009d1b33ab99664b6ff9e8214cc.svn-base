#ifndef __AREA_H__
#define __AREA_H__

#if AREA_JIANGXI

typedef struct
{
    uint16  devID; /* 在点名设备信息数组的下标 */
    uint8   phase;//这个相位是收到信号的相位，实际是儿子所在的相
    uint8   addr[6];
}NODE_INFO_T;   /* 亲缘节点信息 */

typedef struct
{
    uint16  pn;          
    uint8   userType;     //用户类型 213 分支断路器 220 表箱断路器
    uint8   branchType;       //分支类型 1表示一级分支 2表示二级分支
    uint8   addr[6];      //地址
    uint8   baud;
    uint32  port;
    //uint8   failCnt;          //失败次数
    uint32  startTime;           //启动拓扑的时间 
    uint16  parentNum;             //亲代数量，包含所有祖先节点
    NODE_INFO_T parentNode[MAX_BREAK_COUNT];  //设备亲代节点信息
    uint16  sonID; /* 断路器后代节点信息索引 */
    uint16  sonNum;             //后代节点数量
    //NODE_INFO_T sonNode[MAX_BREAK_COUNT];  //设备后代节点信息
    //uint8   isMeterSon; //其儿子是否为表档案
    //NodeType_E nodeType; //节点类型
    uint16  recordNum; //收到的拓扑记录条数
    uint8   readRecordFinish; /* 读拓扑记录完成标志，1为完成 */
    int timeOffset;   //补偿时间
}TOPO_DEV_T; /* 拓扑设备信息 */

#define MAX_TOPO_RECORD_NUM     1024    /* 每个断路器存储识别记录最多条数 */

typedef struct
{
    pthread_rwlock_t    rwLock;         //读写锁 多线程访问
    DB_CLIENT clientId;
    uint32 isInTopo;                     //当前是否在拓扑识别过程中 0表示空闲 1表示正在拓扑工作中
    uint32 readTopoDataFlag;    /* 读拓扑电流识别数据标记，0:禁止读，1:允许读 */
    //Send_Recv_State_T  sendRecvState;   //拓扑识别的收发状态
    //Topoloty_State_T    topoState;      //拓扑识别所处状态
    uint16   delay;              //发送延时
    uint16 breakNum;      //断路器个数
    //uint16 breakIndex;           //当前发送的索引
    uint16 msgIndex;            //发送的消息序号
    //TOPO_DEV_T  break[MAX_BREAK_COUNT];     //断路器信息
    uint16  devNum;    /* 点名设备数量 */
    //uint16  devIndex;    /* 当前点名设备发送索引 */
    TOPO_DEV_T  dev[MTR_MAX_COUNT];     /* 点名设备信息 */
    NODE_INFO_T sonNode[MAX_BREAK_COUNT][MAX_TOPO_RECORD_NUM];  /* 断路器后代节点信息 */
    uint32 overtime;        /* 透传超时时间 */
    uint32  isFinish;        /* 拓扑识别过程是否结束，线损计算入口判断需要用到 */
    struct timespec sendTime;   /* 最近一次拓扑报文发送时间 */
    uint32 sendCheckTime; //发送查询时间报文的时间 
}TOPO_ARG_T;

typedef uint32 (*PackFunc)(TOPO_ARG_T *, uint8 *, uint16 *, uint16);
typedef uint32 (*UnpackFunc)(TOPO_ARG_T *, MeterPrtlFmt_T *, uint16);

void TopoInit();
int ReadTopoData(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
void *TopoThread(void *arg);

#endif

#endif
