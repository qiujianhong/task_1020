#include <sys/soundcard.h>
#include <audio.h>
#include <gpio.h>

/*audio �豸·�� */
#define   AUDIO_DEV_NAME      "/dev/dsp" 
#define   BELL_DEV_NAME       "/dev/gpio"

/*��ƵԴ�ļ���wav��ʽ */
#define   AUDIO_SOURCE_INDEX   "/mnt/wav/index"

/*Ӳ������ */
#define   GPIO_BELL	          SCM_PIN_PH0 

#define AUDIO_DEV_NUM      1
static char *aAudioDevice[AUDIO_DEV_NUM] = {HW_DEVICE_ID_AUDIO};

/*********************************************
 �������ܵ�Ԫ: gpio
 ��������:  set gpio status
 �������: ��
 �������: ��
 ��������ֵ: ��
 �ڲ��������̣�all
**********************************************/
static int32 set_gpio_value(uint32 pin, uint32 value)
{        
    int32 fd;
    int32 ret_value;
    int32 count = 100;
	
    //��
    while(((fd = open(GPIO_DEV_NAME, O_RDONLY))<0)&&((count--)>0))
            usleep(2000);
    
    if (fd < 0)
    {
      printf("can not open gpio device.\n");
      return ERR_NORMAL;
    }
    
    //(0 low | 1 high | <0 error)
    ret_value = ioctl(fd, value, pin);
    
    //�ر�?
    close(fd);
    
    return ret_value;             
										
}


//��Ƶ�豸�������ĳ���
#define AUDIO_BUF_LEN 4096*32
//wav��ʽ�ļ������ͷ�ļ�����
#define HEADMAXBYTES 128

#define AUDIO_FILE_NAME_LEN    128     /*֧�ֵ���Ƶ�ļ�����󳤶ȣ�����·����*/

//wav��ʽ�ļ����ļ�ͷ
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

/* ����Ƶ�豸�ļ� */
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

/* ��ʼ�����ļ� */
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
* @brief ������Ƶ�����豸
* @param[in] dev: �豸����
* @param[in] state: 0 ��ֹ����,1 ʹ�ܲ���
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
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
* @brief ���ŵ�N����Ƶ
* @param[in] dev: �豸����
* @param[in] index: ��Ƶ���
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
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

/***************************************************
* @brief ���������
* @param[in] dev: �豸����
* @param[in] val: 0�� �رգ� 1�� ����
* @param[in] frq: ���Ƶ�ʣ���λ: hz��
* @param[in] count: ����
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
****************************************************/
static int32 audio_bell_ctrl(struct tag_AUDIO_DEVICE *dev,  uint8 val, 
                                 uint32 frq, uint32 count)
{
    if(val == 1)
      set_gpio_value(GPIO_BELL, GPIO_OUT_LOW);
	else if(val == 0)
	  set_gpio_value(GPIO_BELL, GPIO_OUT_HIGH);
	
    return ERR_OK;
}

static int audio_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
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
