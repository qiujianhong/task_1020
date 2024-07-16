#include <sys/soundcard.h>
#include "types.h"
#include <audio.h>
#include <gpio.h>
#include "sysDefine.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <ctype.h>

/*audio 设备路径 */
#define   AUDIO_DEV_NAME      "/dev/dsp" 
#define   BELL_DEV_NAME       "/dev/gpio"

/*音频源文件，wav格式 */
#define   AUDIO_SOURCE_INDEX   "/mnt/wav/index"

/*硬件参数 */
#define   GPIO_BELL	          AT91_PIN_PB21 

#define AUDIO_DEV_NUM      1
static char *aAudioDevice[AUDIO_DEV_NUM] = {HW_DEVICE_ID_AUDIO};

/*********************************************
 所属功能单元: gpio
 功能描述:  set gpio status
 输入参数: 无
 输出参数: 无
 函数返回值: 无
 内部处理流程：all
**********************************************/
static int32 set_gpio_value(uint32 pin, uint32 value)
{        
    int32 fd;
    int32 ret_value;
    int32 count = 100;

    //打开
    while(((fd = open(GPIO_DEV_NAME, O_RDONLY))<0)&&((count--)>0))
            usleep(2000);
    
    if (fd < 0)
    {
      printf("can not open gpio device.\n");
      return ERR_NORMAL;
    }
    
    //(0 low | 1 high | <0 error)
    ret_value = ioctl(fd, value, pin);
    
    //关闭
    close(fd);
    
    return ret_value;
}

#if 0
//音频设备缓冲区的长度
#define AUDIO_BUF_LEN 4096*32
//wav格式文件的最大头文件长度
#define HEADMAXBYTES 128

#define AUDIO_FILE_NAME_LEN    128     /*支持的音频文件名最大长度（包含路径）*/

//wav格式文件，文件头
struct wave_head_t
{
     char format[4];
     unsigned int file_length;
     char format2[12];
     unsigned short wFormatTag;
     unsigned short nChannels;
     unsigned int   nSamplesPerSec;
     unsigned int nAvgBytesPerSec;
     unsigned short nBlockAlign;
     unsigned short wBitsPerSample;
     char format3[4];
     unsigned int nDataBytes; 
};

/* 打开音频设备文件 */
static int audio_open_device(char* devicename)
{
    int handle;
    //open the devices

    if(devicename == NULL)
    {
        printf("-ERR:invalid device name\n");
        return -1;
    }

    handle = open(devicename,O_WRONLY);
    if (handle < 0)
    {
        printf("Failed to open audio device\n" );
        return -1;
    }

    return handle;
}

/* 开始播放文件 */
static int audio_open_wav(char* filename,int* playtime)
{
    int handle;
    if(filename == NULL){
        //close the play
        return -1;
    }

    //check the file
    if(access(filename,F_OK) != 0){
        printf("the wav file is not exist %s\n",filename);
        return -1;
    }

    //printf("open the file\n");
    handle = open(filename,O_RDONLY);
    if(handle < 0){
        printf("cannot open the wav file\t%s\n",filename);
        return -1;
    }

    return handle;
}

//check wav file flag and get wave head
static int audio_get_wave_head(int fd_file, struct wave_head_t* wave_head)
{
    int ret = -1;

    if(wave_head == NULL)
    {
        printf("cannot malloc memory for wave head struct\n");
        return ret;
    }
    
    lseek(fd_file, 0, SEEK_SET);
    ret = read(fd_file, (char*)wave_head, HEADMAXBYTES);
    if (ret < HEADMAXBYTES)
    {
        printf("Read wave file error.\n" );
        return ret;
    }

    if ((wave_head->format[0] != 'R')||(wave_head->format[1] != 'I')
        ||(wave_head->format[2] != 'F')|| (wave_head->format[3] != 'F'))
    {
        printf("File format (1) error.\n" );
        return ret;
    }

    wave_head->format2[7] = 0;
    if (strcmp(wave_head->format2,"WAVEfmt") != 0)
    {
        printf("File format (2) error.\n" );
        return ret;
    }
    return 0;
}

//set file at data offset
//return the data len
static int audio_get_wave_data(int file, struct wave_head_t* wave_head)
{
    char* start;
    char* ptr;
    int len;

    if(audio_get_wave_head(file, wave_head)){
        printf("cannot get wave head\n");
        goto err_free;
    }
    start = (char*)wave_head;
    ptr = & wave_head->format3[0];
    while(*ptr !='d' && (ptr - start) < HEADMAXBYTES ) {ptr++;}

    if(*ptr != 'd') {
        printf("has not format chunk %x,%x,%x,%x\n",ptr[0],ptr[1],ptr[2],ptr[3]);
        goto err_free;
    }
    if ((*ptr++ != 'd')||(*ptr++ != 'a')||(*ptr++ != 't')|| (*ptr++ != 'a'))
    {
        printf("File format (3) is Unknown.\n" );
        goto err_free;
    }
    else
    {
        //printf("File format (3) is data.\n" );
    }
    len = ptr + 4 - start; //head len
    lseek(file, len, SEEK_SET);
    len = (*ptr++);
    len = len + (*ptr++ << 8);
    len = len + (*ptr++<<16);
    len = len + (*ptr++ << 24);
    //len = len & (~3);
    //len = (len <=  (wave_head->file_length - headlen)) ? len : (wave_head->file_length - headlen);
    //printf("Total Data Bytes  = %x\n",len);
    return len;
    
err_free:
    return -1;
}

/***************************************************
* @brief 控制音频播放设备
* @param[in] dev: 设备描述
* @param[in] state: 0 禁止播放,1 使能播放
* @return 成功返回TRUE；失败返回FALSE
****************************************************/
static int32 audio_state_set(struct tag_AUDIO_DEVICE *dev, uint32 state)
{
    int32 fd = 0;
    int32 iRet = ERR_OK;
    
    if(NULL == dev)
    {
        DEBUG("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return ERR_PTR;
    }

    fd = open(AUDIO_DEV_NAME, O_WRONLY);
    if (fd < 0)
    {
        DEBUG("Open audio dev failed\n");
     return ERR_NORMAL;
    }
    if(flock(fd, LOCK_EX))
    {
        DEBUG("audio lock failed\n");
     iRet = ERR_LOCK;
        goto end;
    }

    iRet= ioctl(fd, SOUND_MIXER_WRITE_MUTE, state);
    if(iRet < 0)
    {
        DEBUG("audio ioctl failed\n");
    }

    flock(fd, LOCK_UN);
end:

    close(fd);
    return iRet;
}

/***************************************************
* @brief 播放第N段音频
* @param[in] dev: 设备描述
* @param[in] index: 音频编号
* @return 成功返回TRUE；失败返回FALSE
****************************************************/
static int32 audio_play(struct tag_AUDIO_DEVICE *dev, AUDIO_INDEX_E index)
{
    char filename[AUDIO_FILE_NAME_LEN];
    ///	int length = 0;
    int32 iRet = ERR_OK;
    int32 len = 0;
    int32 temp = 0;
    int32 hWavfile = 0; 
    int32 fd = 0;
    ///char* audio_buf = 0;
    struct wave_head_t* wave_head;
    signed short applicbuf[2048];         
    int count;

    memset(filename, 0, sizeof(filename));
    
    fd = audio_open_device(AUDIO_DEV_NAME);
    if(fd <= 0)
    {
        printf("open the devifle err\n");
        goto end;
    }

    switch(index)
    {
        case EMAUDIO_INDEX_0:
        {
            temp = 0;
            ioctl(fd,SOUND_MIXER_WRITE_MUTE,&temp);
            goto end;
        }
        default:
        {
            break;
        }
    }

    sprintf(filename, AUDIO_SOURCE_INDEX"%d"".wav", index);
    hWavfile = audio_open_wav(filename,NULL);
    if(hWavfile <= 0)
    {
        hWavfile = 0;
        goto end;
    }

    wave_head = (struct wave_head_t*)malloc(HEADMAXBYTES);

    len = audio_get_wave_data(hWavfile, wave_head);
    if(len <=0)
    {
        goto end;
    }
    //set audio device
    temp = 1;
    ioctl(fd, SOUND_MIXER_WRITE_MUTE, &temp);

    temp = wave_head->nSamplesPerSec;
    if(temp < 11025) {
        printf("Not support Samples Per Sec < 11025\n");
    }
    ioctl(fd,SNDCTL_DSP_SPEED,&temp);

    temp = wave_head->nChannels;
    ioctl(fd,SNDCTL_DSP_CHANNELS,&temp);

    free(wave_head);

    while ((count = read(hWavfile, applicbuf, 2048)) > 0) 
    {   
        write(fd, applicbuf, count);
    }
    
end:
    close(fd);
    close(hWavfile);
    
    return iRet;
}
#else
//播放线程ID
static pthread_t play_tid=0;

//播放线程结束标志
static int playing_flag = 0;

//打开时的互斥变量
static pthread_mutex_t open_mutex = PTHREAD_MUTEX_INITIALIZER;

/***************************************************
* @brief      音频播放线程
* @param[in]  data: 播放文件
* @param[out] 
* @return 
****************************************************/
static void* audio_play_thread (void* data)
{
    char* filename = (char*) data;
    char run_msg[256] = {0};

    playing_flag = 1;
    play_tid = pthread_self();

    set_gpio_value(AT91_PIN_PB21, GPIO_OUT_HIGH);
    
    sprintf(run_msg, "aplay %s", filename);
    printf(" %s thread id:%ld\n", run_msg, play_tid);
    system(run_msg);

    set_gpio_value(AT91_PIN_PB21, GPIO_OUT_LOW);

    pthread_mutex_unlock(&open_mutex);

    pthread_exit(NULL);
    printf("pthread_exit\n");
}

/***************************************************
* @brief      音频播放接口
* @param[in]  filename: 播放文件
* @param[out] 
* @return 错误码
****************************************************/
static int audio_wav_paly(char *filename)
{
    int err;
    pthread_t temp_tid;

    printf("WAVEPlay %s\n", filename);
    pthread_mutex_lock(&open_mutex);
    printf("start thread\n");
    if (play_tid)
    {
        err = pthread_join(play_tid, NULL);
        play_tid = 0;
        if (err) 
        {
            printf("pthread_join for exit failed. with error %d\n",err);
            pthread_mutex_unlock(&open_mutex);
            return -1;
        }
    } 

    playing_flag = 0;

    err = pthread_create(&temp_tid, NULL, audio_play_thread, filename);
    if (err != 0) 
    {
        printf("play wav thread Create error!\n");
        return -1;
    }

    return 0;
}

/***************************************************
* @brief 控制音频播放设备
* @param[in] dev: 设备描述
* @param[in] state: 0 禁止播放,1 使能播放
* @return 错误码
****************************************************/
static int32 audio_state_set(struct tag_AUDIO_DEVICE *dev, uint32 state)
{
    int32 iRet = ERR_OK;
    
    if (state == 1)
    {
        iRet = set_gpio_value(AT91_PIN_PB21, GPIO_OUT_HIGH);
    }
    else if (state == 0)
    {
        iRet = set_gpio_value(AT91_PIN_PB21, GPIO_OUT_LOW);
    }
    else
    {
        iRet = ERR_NORMAL;
    }

    return iRet;
}

/***************************************************
* @brief 进程检查
* @param[in] 进程名称
* @param[in] 
* @return 存在TRUE；不存在FALSE
****************************************************/
static bool process_check_state(const char* psProcessName)
{
    FILE* fp;
    char buf[512] = {0};
    char cmd[128] = {0};
    uint8 isExist = FALSE;
        
    sprintf(cmd, "ps | grep %s | grep -v grep", psProcessName);
    fp = popen(cmd, "r");   
    if (NULL == fp)   
    {   
        return FALSE;   
    }
    
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        if (strstr(buf, psProcessName) != NULL)
        {
            isExist = TRUE;
            break;
        }
    }

    pclose(fp);

    return isExist;

}

/***************************************************
* @brief 杀掉进程
* @param[in] 进程名称
* @param[in] 
* @return 存在TRUE；不存在FALSE
****************************************************/
static void process_kill(const char* psProcessName)
{
    char cmd[128] = {0};
    sprintf(cmd, "killall %s", psProcessName);
    system(cmd);

    return;
}

/***************************************************
* @brief 播放第N段音频
* @param[in] dev: 设备描述
* @param[in] index: 音频编号
* @return 错误码
****************************************************/
static int32 audio_play(struct tag_AUDIO_DEVICE *dev, AUDIO_INDEX_E index)
{
    uint8 count = 0;
    int32 iRet = ERR_OK;

    //如果索引为0则关闭音频
    if (index == 0)
    {
        set_gpio_value(AT91_PIN_PB21, GPIO_OUT_LOW);
        process_kill("aplay");
        return ERR_OK;
    }

    while (1)
    {
        if (count >= 8)
        {
            return ERR_TIMEOUT;
        }

        if (process_check_state("aplay"))
        {
            count ++;
        }
        else
        {
            break;
        }

        set_gpio_value(AT91_PIN_PB21, GPIO_OUT_LOW);
        process_kill("aplay");
        sleep(1);
    }
    
    switch (index)
    {
    case EMAUDIO_INDEX_1: //运行参数已更改，请注意查看
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/S10000.wav");
        break;
    }
    
    case EMAUDIO_INDEX_2: //最新信息，请查看
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/S10001.wav");
        break;
    }
    
    case EMAUDIO_INDEX_3: //时段控已投入
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Pc20000.wav");
        break;
    }

    case EMAUDIO_INDEX_4: //时段控已解除
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Pc20001.wav");
        break;
    }

    case EMAUDIO_INDEX_5: //厂休控已投入
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Pc20002.wav");
        break;
    }

    case EMAUDIO_INDEX_6: //厂休控已解除
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Pc20003.wav");
        break;
    }

    case EMAUDIO_INDEX_7: //营业报停控投入
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Yy10000.wav");
        break;
    }

    case EMAUDIO_INDEX_8: //营业报停控解除
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Yy10001.wav");
        break;
    }

    case EMAUDIO_INDEX_9: //月电控已投入
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Ec20000.wav");
        break;
    }

    case EMAUDIO_INDEX_10://月电控已解除
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Ec20001.wav");
        break;
    }

    case EMAUDIO_INDEX_11://购电控已投入
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Ec20002.wav");
        break;
    }

    case EMAUDIO_INDEX_12://购电控已解除
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Ec20003.wav");
        break;
    }

    case EMAUDIO_INDEX_13://当前功率下浮控已投入
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Pc20004.wav");
        break;
    }

    case EMAUDIO_INDEX_14://当前功率下浮控已解除
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Pc20005.wav");
        break;
    }

    case EMAUDIO_INDEX_15://遥控跳闸
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/K20000.wav");
        break;
    }

    case EMAUDIO_INDEX_16://允许合闸
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/K20001.wav");
        break;
    }

    case EMAUDIO_INDEX_17://已超负荷，请限电
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Pc20007.wav");
        break;
    }

    case EMAUDIO_INDEX_18://电量快用完，请速购电
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Ec20004.wav");
        break;
    }

    case EMAUDIO_INDEX_19://电量用完，已跳闸
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Ec20005.wav");
        break;
    }

    case EMAUDIO_INDEX_20://嘀(报警音,1秒) 
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/XL0E.wav");
        break;
    }

    case EMAUDIO_INDEX_21://请用户及时结算电费
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Js10000.wav");
        break;
    }
    
    case EMAUDIO_INDEX_22://工控跳闸
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/Pc20006.wav");
        break;
    }

    case EMAUDIO_INDEX_25://远程合闸
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/K20003.wav");
        break;
    }
        
    default:
    {
        iRet = audio_wav_paly("/lib/hal_lib/wav/XL0E.wav");
        break;
    }
    }

    if (iRet != 0)
    {
        return ERR_NORESOURCE;
    }
    else
    {
        return ERR_OK;
    }
}
#endif

/***************************************************
* @brief 蜂鸣器输出
* @param[in] dev: 设备描述
* @param[in] val: 0： 关闭， 1： 开启
* @param[in] frq: 输出频率（单位: hz）
* @param[in] count: 次数
* @return 成功返回TRUE；失败返回FALSE
****************************************************/
static int32 audio_bell_ctrl(struct tag_AUDIO_DEVICE *dev,  uint8 val, 
                                 uint32 frq, uint32 count)
{
    //if(val == 1)
    //  set_gpio_value(GPIO_BELL, GPIO_OUT_LOW);
    //else if(val == 0)
    //  set_gpio_value(GPIO_BELL, GPIO_OUT_HIGH);
    return ERR_OK;
}

static int audio_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
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
