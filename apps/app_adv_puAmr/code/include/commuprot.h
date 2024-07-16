/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块通道定义
* @date：    2019-11-8
* @history： 
*********************************************************************
*/

#ifndef _COMMUPROT_H_
#define _COMMUPROT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* 通信介质 */
#define COMM_MEDIA_BLUETOOTH      0    //蓝牙通信介质
#define COMM_MEDIA_GSM            1    //短信通信介质
#define COMM_MEDIA_GPRS           2    //无线通信介质
#define COMM_MEDIA_ETH            3    //以太网通信介质
#define COMM_MEDIA_LISTEN         4    //监听通信介质
#define COMM_MEDIA_485            5    //485通信介质
#define COMM_MEDIA_IR             6    //红外通信介质
#define COMM_MEDIA_232            7    //232通信介质
#define COMM_MEDIA_RADIO          8    //电台通信介质
#define COMM_MEDIA_APP            9    //APP通信介质


/* 通道大类定义 需要和内部通信配合 */
#define CHANNEL_DEFAULT             0x00        /*缺省通道*/
#define LOCATE_232                  0x01        /*本地232通道*/
#define INFRARED                    0x02        /*红外*/
#define JOIN485_1                   0x03        /*485级联1*/
#define JOIN485_2                   0x04        /*485级联2*/
#define COMM_LISTEN                 0x05        /*本地以太网监听信道*/
#define CHL_RADIO                   0x06        /*电台*/
#define CHL_CAN_1                   0x07        /*can通道1*/
#define CHL_CAN_2                   0x08        /*can通道2*/
#define CHL_PLC_1                   0x09        /*载波通道*/
#define CHL_APP                     0x0a        /*APP透传*/
#define CHL_MANUAL_AMR              0x0B        /* GUI收到抄表测试通道 */
#define LOCATE_BLUE                 0x0C        /*蓝牙 内部排号 0xC-0xD */
#define CHL_ACMETER                 0x0E        /* 交采透传 */


#define CHL_GPRS_1                  0x10        //无线通道1,  内部排号 0x10-0x29
#define CHL_GSM_1                   0x2a        //短信通道1,  内部排号 0x2a-0x2d
#define CHL_GPRS_2                  0x30        //无线通道2   内部排号 0x30-0x49
#define CHL_GSM_2                   0x4a        //短信通道2,  内部排号 0x4a-0x4d

#define CHL_ETH_1                   0x50        //以太网通道1 内部排号 0x50-0x6f
#define CHL_ETH_2                   0x70        //以太网通道2 内部排号 0x70-0x8f

#define CHL_RESERVE                 0x90        //保留

#define MAX_CHANNEL_NUM             CHL_RESERVE

/* 内部协议 */
#define PRO_DATA_TRANSMIT         0     /*内部协议 - 数据中转传送*/
#define PRO_LOGIN_FRAME           1     /*内部协议 - 登陆帧*/
#define PRO_EXIT_LOGIN_FRAME      2     /*内部协议 - 退出登陆*/
#define PRO_HEART_FRAME           3     /*内部协议 - 心跳帧*/
#define PROT_OPEN_CHANNLE         4     /*内部协议 - 打开通道*/
#define PRO_PARA_MOD              5     /*内部协议 - 通讯参数修改*/
#define PROT_CLOSE_CHANNLE        6     /*内部协议 - 关闭通道*/
#define PROT_OPEN_ACK             7     /*内部协议 - 打开通道后确认消息*/
#define PROT_CLOSE_ACK            8     /*内部协议 - 关闭通道后确认消息*/
#define PROT_APPMAIN_START        10    /*内部协议 - appmain启动 获取 通道开启数据*/
#define PROT_COMM_START           11    /*内部协议 - appmain启动 获取 通道开启数据*/
#define PRO_LOGIN_ERROR           12    /*内部协议 - 登陆失败*/
#define PRO_HEARTBEAT_ERROR       13    /*内部协议 - 心跳丢失失败*/
#define PRO_BIG_MSG               14    /*内部协议 - 大消息发送*/
#define PROT_PAUSE_CHANNLE        15    /*内部协议 - 暂停通道 */
#define PROT_CONTINUE_CHANNLE     16    /*内部协议 - 继续通道 */
#define PROT_LISTEN_WORK          18    /*内部协议 - 通道监听工作 */
#define PROT_LISTEN_PAUSE         19    /*内部协议 - 通道监听停止 */



typedef struct tagCommIpcMsg
{
    uint8 id;       //通道
    uint8 protocol; //内部协议
    uint8 len1;     //长度1
    uint8 len2;     //长度2
    uint8 msg[0];   //消息内容
}COMM_IPCMSG_T;

#define CHANNEL_WORKMODE_DEFAULT   0xff
#define CHANNEL_MIN_HEART          60      //心跳最小发送60s 防止终端频繁登录
/* 通道属性 */
typedef struct tagChannelInfo
{
    char   SMSMaster[32]; //短信主站号码
    uint32 ip;    //主站Port
    uint16 port;  //主站port
    uint8  workmode; //工作模式 1-客户端、2-服务器、0-混合
    uint8  medium;   //通信介质
    uint8  connType; //连接方式   1-UDP 0-TCP
    uint8  bFirst;   //是否拨号后首次建立的连接
    uint16 heart;     //心跳周期(秒)
    uint8  dailcnt;  //重发次数
    uint8  dailtime; //超时时间
    uint8  res[2];
    uint16 proxyPort;//通信前置机端口
    uint32 proxyip;  //通信前置机地址
}CHANNEL_INFO_T;

typedef struct tagCommBigMsg
{
    uint32 msglen;   //消息长度
    uint8 *msgaddr;  //消息地址
    time_t packtime; //组包时间 超过8s放弃操作
    uint8  msgindex; //消息索引
    uint8  res[3];
}COMM_BIGMSG_T;


#ifdef __cplusplus
}
#endif
#endif
