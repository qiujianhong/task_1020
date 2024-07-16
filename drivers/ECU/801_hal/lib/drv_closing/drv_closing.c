#include "types.h"
#include <drv_closing.h>
#include "sysDefine.h"
#include <pthread.h>     //线程函数库定义


#define DRV_CLOSINGDEV_NUM          1
static char *aDrvClosingDevice[DRV_CLOSINGDEV_NUM] = {HW_DEVICE_ID_DRV_CLOSING};

typedef struct tagCtrlSwitchNode
{
    uint8   trip;   //跳闸
    uint8   close;  //合闸
    uint8   bAlarm;
}CTRL_SWITCH_NODE_T;

#define CTRL_SWITCH_LIST_MAX     60
typedef struct tagCtrlSwitchList
{
    uint16             head;                         /*队列头*/
    uint16             tail;                         /*队列尾*/
    CTRL_SWITCH_NODE_T queue[CTRL_SWITCH_LIST_MAX];
}CTRL_SWITCH_LIST_T;

#define PREPARE_ON                      0x11
#define PREPARE_OFF                     0x7F
#define SET_MID_BOARD_YK_DAT            0x11
#define SET_MID_BOARD_YD_DAT            0x22

#define   LED_DEV_NAME      "/dev/led" 

/**********************************************************************
* @name      : appctrl_IIC_Prepare
* @brief     ：中板遥控预动（合闸继电器）
* @param[in] ：uint8         yd_out (bit6是告警)
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-7-28
* @Update    :
**********************************************************************/
static int closing_IIC_Prepare(int fd, uint8 yd_out)
{
    if(fd <= 0)
    {
        return -1;
    }
    
    //写入预动状态
    if ( ioctl(fd, SET_MID_BOARD_YD_DAT, yd_out) < 0)
    {
        DEBUG("合闸预动失败\n");
        return -1;
    }
    
    return 0;
}

/**********************************************************************
* @name      : appctrl_IIC_Contactor
* @brief     ：中板遥控（合闸继电器）
* @param[in] ：uint8         ctrl_out (bit0-bit7分别表示1-8路)
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-7-28
* @Update    :
**********************************************************************/
static int closing_IIC_Contactor(int fd, uint8 ctrl_out)
{
    if(fd <= 0)
    {
        return -1;
    }
    
    //写入控制状态
    if ( ioctl(fd, SET_MID_BOARD_YK_DAT, ctrl_out) < 0)
    {
        DEBUG("合闸设置失败\n");
        return -1;
    }

    return 0;
}

static int led_state(int index, int state)
{
    int fd = 0;
    int iRet = ERR_OK;
    
    fd = open(LED_DEV_NAME, O_RDWR);
    if (fd < 0) 
    { 
        return ERR_NORMAL; 
    }
    
    /*控制LED亮灭 */
    iRet = ioctl(fd, state, index-1);
    if(iRet < 0)
    {
        printf("led ioctl failed\n");
        iRet = ERR_NORMAL;
    }
    close(fd);

    return iRet;
}


/**********************************************************************
* @name      : closing_switch_later
* @brief     ：控制延后生效线程(目前只实现脉冲控制)
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2024-2-19
* @Update    :
**********************************************************************/
void *closing_switch_later(void *pArg)
{
    DRV_CLOSING_DEVICE_T* dev = (DRV_CLOSING_DEVICE_T*)pArg;
    CTRL_SWITCH_LIST_T  * pList = NULL;
    CTRL_SWITCH_NODE_T    tTemp;
    CTRL_SWITCH_NODE_T    tNode;
    uint16                count = 0;
    int                   ret = 0;
    uint8                 zAlarm = 0x10;
    struct timeval  tv;
    struct timezone tz;

    if(NULL == dev || NULL == dev->list)
    {
        return 0;
    }
    
    pList = (CTRL_SWITCH_LIST_T*)dev->list;

    while(1)
    {
        usleep(100000);
        if(1 == dev->stop_flag)
        {
            return 0;
        }

        if(pList->head == pList->tail)
        {
            continue;
        }

        memset(&tNode, 0, sizeof(CTRL_SWITCH_NODE_T));
        /* 合并 */
        while(pList->head != pList->tail)
        {
            memcpy(&tTemp, &(pList->queue[pList->head]), sizeof(CTRL_SWITCH_NODE_T));
        
            pList->queue[pList->head].trip = 0;
            pList->queue[pList->head].close = 0;
            pList->queue[pList->head].bAlarm = 0;
            pList->head = (pList->head + 1) % CTRL_SWITCH_LIST_MAX;

            tNode.close |= tTemp.close;
            tNode.bAlarm |= tTemp.bAlarm;

            usleep(10000);
        }

        if(1 == tNode.bAlarm)
        {
            tNode.close |= zAlarm;
        }
        if(0 == tNode.close)
        {
            continue;
        }

        /* 预动 */
        closing_IIC_Prepare(dev->fd, PREPARE_ON);
        count = 0;
        while(count++ < 8)
        {
            usleep(100000);
            if(1 == dev->stop_flag)
            {
                closing_IIC_Prepare(dev->fd, PREPARE_OFF);
                return 0;
            }
        }

        /* 继电器动作 */
        gettimeofday(&tv, &tz);
        DEBUG("(%ld.%03ld)合闸操作 %02X\n", tv.tv_sec, tv.tv_usec/1000, tNode.close);
        
        ret = closing_IIC_Contactor(dev->fd, tNode.close);
        usleep(dev->pulse_width * 1000); //脉冲宽度
        
        /* 结束 */
        if(TRUE == tNode.bAlarm)
        {
            ret |= closing_IIC_Contactor(dev->fd, zAlarm);
            gettimeofday(&tv, &tz);
            DEBUG("(%ld.%03ld)合闸操作 10\n", tv.tv_sec, tv.tv_usec/1000);
        }
        else
        {
            ret |= closing_IIC_Contactor(dev->fd, 00);
            gettimeofday(&tv, &tz);
            DEBUG("(%ld.%03ld)合闸操作 00\n", tv.tv_sec, tv.tv_usec/1000);
        }

        if(TRUE == tNode.bAlarm)
        {
            sleep(1); //告警
            ret |= closing_IIC_Contactor(dev->fd, 0);
            gettimeofday(&tv, &tz);
            DEBUG("(%ld.%03ld)告警合闸操作 00\n", tv.tv_sec, tv.tv_usec/1000);
        }

        /* 结束预动 */
        closing_IIC_Prepare(dev->fd, PREPARE_OFF);

        if(0 != ret)
        {
            close(dev->fd);
            dev->fd = -1;
            dev->fd = open("/dev/ykyd", O_RDONLY);
        }

        usleep(100000);
    }

    return 0;
}

/**
* @brief 设置合闸继电器输出模式
* @param[in] dev: 设备描述
* @param[in] mode: 合闸模式
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_mode_set(struct tag_CLOSING_DEVICE *dev, CLOSING_MODE_E mode)
{
    if(NULL == dev)
    {
        return -1;
    }
    dev->mode = mode;
    return 0;
}

/**
* @brief 设置合闸继电器输出脉冲宽度
* @param[in] dev: 设备描述
* @param[in] period: 合闸继电器输出脉冲宽度，单位：ms
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_pulse_width_set(struct tag_CLOSING_DEVICE *dev, uint32 pulse_width)
{
    if(NULL == dev)
    {
        return -1;
    }
    dev->pulse_width = pulse_width;
    return 0;
}

/**
* @brief 获取跳闸接线状态
* @param[in] dev: 设备描述
* @param[out] state: 接线状态，bit0…bit(n-1)按位表示1~n 轮接线状态，1有接线，0 没接线
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_line_state_get(struct tag_CLOSING_DEVICE *dev, uint32 *state)
{
    uint8  newstatus = 0;
    uint32 data1 = 1;
    uint32 data2 = 2;
    uint32 data3 = 3;
    uint32 data4 = 4;
    uint32 data5 = 5;
    int bReadCount = 0;
    int fd = -1;

    if(NULL == dev || NULL == state)
    {
        return ERR_PTR;
    }

    fd = open("/dev/ym", O_RDONLY|O_NONBLOCK);
    if (fd <= 0)
    {
        return ERR_NOTEXIST;
    }
    
    while(bReadCount++ < 10)
    {
        read(fd, &data1, sizeof(data1));
        data1= ((data1 >> 25) & 0x0F);
        usleep(1000);
        
        read(fd, &data2, sizeof(data2));
        data2= ((data2 >> 25) & 0x0F);
        usleep(1000);
        
        read(fd, &data3, sizeof(data3));
        data3= ((data3 >> 25) & 0x0F);
        usleep(1000);
        
        read(fd, &data4, sizeof(data4));
        data4= ((data4 >> 25) & 0x0F);
        usleep(1000);
        
        read(fd, &data5, sizeof(data5));
        data5= ((data5 >> 25) & 0x0F);
        
        //连续5次读到相同的值，防止干扰
        if(data1 == data2 && data1 == data3 && data1 == data4 && data1 == data5)
        {
            newstatus = data1;
            break;
        }
    }
    close(fd);
    if(bReadCount >= 10)
    {
        return ERR_NORMAL;
    }

    *state = ~newstatus;
    return ERR_OK;
}

/**
* @brief合闸继电器输出控制
* @param[in] dev: 设备描述
* @param[in] id: 1~4为4轮合闸
* @param[in] turn: 动作控制
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_turn(struct tag_CLOSING_DEVICE *dev, uint8 id, CLOSING_TURN_E turn)
{
    CTRL_SWITCH_LIST_T  *pList = NULL;
    uint8                status = 0;
    int                  readindex = 0;
    int                  greenindex = 0;
    
    if(NULL == dev || NULL == dev->list || id == 0 || id > 4)
    {
        return -1;
    }
    
    pList = (CTRL_SWITCH_LIST_T*)dev->list;

    /* 跳闸 */
    if(CLOSING_TURN_ON == turn)
    {
        switch(id)
        {
            case 1: status = 0x01; break;
            case 2: status = 0x02; break;
            case 3: status = 0x04; break;
            case 4: status = 0x08; break;
            default: break;
        }
    }
    
    pList->queue[pList->tail].trip = 0x00;
    pList->queue[pList->tail].close = status;
    pList->queue[pList->tail].bAlarm = 0;
    pList->tail = (pList->tail + 1) % CTRL_SWITCH_LIST_MAX;

    /* LED控制 */
    readindex = 2 * id + 1;
    greenindex = 2 * id + 2;
    led_state(readindex, CLOSING_TURN_ON == turn ? 1 : 0);
    led_state(greenindex, CLOSING_TURN_ON == turn ? 0 : 1);
    return 0;
}

/**
* @brief告警继电器输出控制
* @param[in] dev: 设备描述
* @param[in] turn: 动作控制
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_turn_alarm(struct tag_CLOSING_DEVICE *dev, CLOSING_TURN_E turn)
{
    CTRL_SWITCH_LIST_T  *pList = NULL;
    uint8                status = 0;
    
    if(NULL == dev || NULL == dev->list)
    {
        return -1;
    }
    
    pList = (CTRL_SWITCH_LIST_T*)dev->list;

    /* 跳闸 */
    if(CLOSING_TURN_ON == turn)
    {
        status = 1;
    }
    
    pList->queue[pList->tail].trip = 0x00;
    pList->queue[pList->tail].close = 0x00;
    pList->queue[pList->tail].bAlarm = status;
    pList->tail = (pList->tail + 1) % CTRL_SWITCH_LIST_MAX;

    return 0;
}

static int drv_closing_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    pthread_t          nThreadId;
    DRV_CLOSING_DEVICE_T* dev = (DRV_CLOSING_DEVICE_T*)malloc(sizeof(DRV_CLOSING_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }

    CTRL_SWITCH_LIST_T *switch_list = (CTRL_SWITCH_LIST_T*)malloc(sizeof(CTRL_SWITCH_LIST_T));
    if(switch_list == NULL)
    {
        free(dev);
        return ERR_NORMAL;
    }
    
    memset(switch_list, 0, sizeof(CTRL_SWITCH_LIST_T));
    memset(dev, 0, sizeof(DRV_CLOSING_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->closing_mode_set = closing_mode_set;
    dev->closing_pulse_width_set = closing_pulse_width_set;
    dev->closing_line_state_get = closing_line_state_get;
    dev->closing_turn = closing_turn;
    dev->closing_turn_alarm = closing_turn_alarm;
    
    dev->fd = open("/dev/ykyd", O_RDONLY);
    if(dev->fd < 0)
    {
        free(dev);
        return ERR_NORMAL;
    }
    
    dev->mode = CLOSING_PULSE;  //默认脉冲模式
    dev->pulse_width = 300;     //默认脉宽300ms
    dev->list = switch_list;

    closing_IIC_Contactor(dev->fd, 0x00);
    closing_IIC_Prepare(dev->fd, PREPARE_OFF);
    pthread_create(&nThreadId, NULL, closing_switch_later, dev);
    dev->threadid = nThreadId;
    *device = (HW_DEVICE*)dev;

    return ERR_OK;
}

static int drv_closing_close(HW_MODULE *pModule, HW_DEVICE *pDevice)
{
    DRV_CLOSING_DEVICE_T *dev = (DRV_CLOSING_DEVICE_T*)pDevice;

    /* 停止线程 */
    dev->stop_flag = 0;
    usleep(100000 + dev->pulse_width * 1000);
    pthread_cancel(dev->threadid);
    pthread_join(dev->threadid, NULL);
    
    if(dev->list)
    {
        free(dev->list);
    }

    //取消动作
    closing_IIC_Contactor(dev->fd, 0x00);
    closing_IIC_Prepare(dev->fd, PREPARE_OFF);

    if(dev->fd > 0)
    {
        close(dev->fd);
        dev->fd = -1;
    }
    
    free(dev);

    return ERR_OK;
}

/* 初始化模块变量 */
DRV_CLOSING_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "drv_closing",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2024-02-19",
        .nDevNum  = DRV_CLOSINGDEV_NUM,
        .szDevices = aDrvClosingDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = drv_closing_open,
        .pfClose = drv_closing_close,
    },
};
