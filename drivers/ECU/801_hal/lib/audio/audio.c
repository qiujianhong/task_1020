#include <sys/soundcard.h>
#include "types.h"
#include <audio.h>
#include <gpio.h>
#include "sysDefine.h"

#define DEV_PATH_AUDIO "/dev/pwm4"
///< ioctl
#define XAUDIO_SETBEEP_CMD  IOCTL_MACRO('a', 2, WRITE_DATA_CMD, 4)   ///< ����������

typedef struct
{
    void *dev;
    unsigned int freq;
    unsigned int duty;
} pwm_param_t;

#define PWM_SETSTATE_CMD IOCTL_MACRO('p', 1, WRITE_DATA_CMD, 4)     ///< ���ܿ���(����Ϊ�ߵ�ƽ)
#define PWM_SETPARAM_CMD IOCTL_MACRO('p', 2, WRITE_DATA_CMD, sizeof(pwm_param_t))  ///< ���ò���
#define PWM_GETPARAM_CMD IOCTL_MACRO('p', 3, READ_DATA_CMD, sizeof(pwm_param_t))   ///< ��ȡ����


#define AUDIO_DEV_NUM      1
static char *aAudioDevice[AUDIO_DEV_NUM] = {HW_DEVICE_ID_AUDIO};

/***************************************************
* @brief ������Ƶ�����豸
* @param[in] dev: �豸����
* @param[in] state: 0 ��ֹ����,1 ʹ�ܲ���
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
****************************************************/
static int32 audio_state_set(struct tag_AUDIO_DEVICE *dev, uint32 state)
{
    return 0;
}

/***************************************************
* @brief ���ŵ�N����Ƶ
* @param[in] dev: �豸����
* @param[in] index: ��Ƶ���
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
****************************************************/
static int32 audio_play(struct tag_AUDIO_DEVICE *dev, AUDIO_INDEX_E index)
{
    return 0;
}

/***************************************************
* @brief ���������
* @param[in] dev: �豸����
* @param[in] val: 0�� �رգ� 1�� ����
* @param[in] frq: ���Ƶ�ʣ���λ: hz��
* @param[in] count: ����
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
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

    // ����cmdΪPWM_SETSTATE_CMD��ֵΪ1����PWM�豸��ֵΪ0���ر�pwm�豸
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

    pwm_param.freq = 2000;  // Ƶ��Ϊ2kHz����Ӧ0.5����һ�θߵ͵�ƽ�л�
    pwm_param.duty = 10;     // ռ�ձ����á�ʵ��ռ�ձ�Խ�ͣ�����Խ��1-100��Ч��
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
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */
    
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

/* ��ʼ��ģ����� */
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
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = audio_open,
        .pfClose = audio_close,
    },
};
