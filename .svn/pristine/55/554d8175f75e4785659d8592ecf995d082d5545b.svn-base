#include <sys/soundcard.h>
#include "types.h"
#include <audio.h>
#include <gpio.h>
#include "sysDefine.h"

#define DEV_PATH_AUDIO "/dev/pwm4"
///< ioctl
#define XAUDIO_SETBEEP_CMD  IOCTL_MACRO('a', 2, WRITE_DATA_CMD, 4)   ///< 蜂鸣器控制

typedef struct
{
    void *dev;
    unsigned int freq;
    unsigned int duty;
} pwm_param_t;

#define PWM_SETSTATE_CMD IOCTL_MACRO('p', 1, WRITE_DATA_CMD, 4)     ///< 功能开关(设置为高电平)
#define PWM_SETPARAM_CMD IOCTL_MACRO('p', 2, WRITE_DATA_CMD, sizeof(pwm_param_t))  ///< 设置参数
#define PWM_GETPARAM_CMD IOCTL_MACRO('p', 3, READ_DATA_CMD, sizeof(pwm_param_t))   ///< 读取参数


#define AUDIO_DEV_NUM      1
static char *aAudioDevice[AUDIO_DEV_NUM] = {HW_DEVICE_ID_AUDIO};

/***************************************************
* @brief 控制音频播放设备
* @param[in] dev: 设备描述
* @param[in] state: 0 禁止播放,1 使能播放
* @return 成功返回TRUE；失败返回FALSE
****************************************************/
static int32 audio_state_set(struct tag_AUDIO_DEVICE *dev, uint32 state)
{
    return 0;
}

/***************************************************
* @brief 播放第N段音频
* @param[in] dev: 设备描述
* @param[in] index: 音频编号
* @return 成功返回TRUE；失败返回FALSE
****************************************************/
static int32 audio_play(struct tag_AUDIO_DEVICE *dev, AUDIO_INDEX_E index)
{
    return 0;
}

/***************************************************
* @brief 蜂鸣器输出
* @param[in] dev: 设备描述
* @param[in] val: 0： 关闭， 1： 开启
* @param[in] frq: 输出频率（单位: hz）
* @param[in] count: 次数
* @return 成功返回TRUE；失败返回FALSE
****************************************************/
static int32 audio_bell_ctrl(struct tag_AUDIO_DEVICE *dev,  uint8 val, uint32 frq, uint32 count)
{
    int32 fd = -1;
    pwm_param_t pwm_param;
    unsigned long value = val;

    if(dev == NULL)
    {
        return ERR_PNULL;
    }
    
    fd = open(DEV_PATH_AUDIO, O_RDWR);
    if(-1 == fd)
    {
        DEBUG("Open failed %s\n", DEV_PATH_AUDIO);
        return ERR_NODEV;
    }

    // 配置cmd为PWM_SETSTATE_CMD，值为1，打开PWM设备，值为0，关闭pwm设备
    if (ioctl(fd, PWM_SETSTATE_CMD, &value) == -1) 
    {
        DEBUG("Open or Close failed\n");
        close(fd);
        return ERR_NODEV;
    }

    if(0 == value)
    {
        close(fd);
        return 0;
    }

    pwm_param.freq = 2000;  // 频率为2kHz，对应0.5毫秒一次高低电平切换
    pwm_param.duty = 10;     // 占空比配置。实测占空比越低，声音越大（1-100有效）
    if(ioctl(fd, PWM_SETPARAM_CMD, &pwm_param) == -1)
    {
        DEBUG("Set Audio failed\n");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}

static int audio_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    AUDIO_DEVICE_T* dev = (AUDIO_DEVICE_T*)malloc(sizeof(AUDIO_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    
    memset(dev, 0, sizeof(AUDIO_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_AUDIO;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->audio_state_set = audio_state_set;
    dev->audio_play = audio_play;
    dev->audio_set_buzzer_out = audio_bell_ctrl;
        
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int audio_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    AUDIO_DEVICE_T *dev = (AUDIO_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
AUDIO_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "audio",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-18",
        .nDevNum = AUDIO_DEV_NUM,
        .szDevices = aAudioDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = audio_open,
        .pfClose = audio_close,
    },
};
