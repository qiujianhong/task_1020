#include "types.h"
#include <key.h>
#include "sysDefine.h"

/*key �豸·�� */
#define   KEY_DEV_NAME      "/dev/key"
#define   KEYRELEASE          0x80        //KEY release flag
//��¼�����һ�ΰ���ʱ��ϵͳ����ʱ��
#define  KEY_PRESS_TIME_FILE  "/tmp/dev/lst_press_time"

#define KEY_DEV_NUM      1
static char *aKeyDevice[KEY_DEV_NUM] = {HW_DEVICE_ID_KEY};

/**
* @brief �����һ�ΰ���ʱ��ϵͳ����ʱ��д���ļ�
* @param[in] 
* @param[out] 
* @note
* @return�ɹ�����ERR_OK��ʧ�ܷ��ش�����
*/
int32 key_press_time_write()
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;
    struct timespec tp;
    uint32 lst_press_time;
    fd = open(KEY_PRESS_TIME_FILE, O_CREAT|O_WRONLY,0666);
    if(fd < 0)
    {
        printf("open %s error!\n", KEY_PRESS_TIME_FILE);
        return ERR_NORMAL;   
    }

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    
    iRet = fcntl(fd, F_SETLKW, &lock);
    
    if(iRet != 0)
        printf("fcntl error ret:%d\n", iRet);

    //��ȡϵͳ����ʱ��
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    lst_press_time = tp.tv_sec;
    printf("lst_press_time:%d\n", lst_press_time);
    
    iRet = write(fd, &lst_press_time, sizeof(uint32));
    
    if(iRet != sizeof(uint32))
    {
        printf("lst_press_time write error\n");
        iRet = ERR_NORMAL;
    }
    else
    {
        iRet = ERR_OK;
    }

    lock.l_type = F_UNLCK;
    iRet = fcntl(fd, F_SETLK, &lock);
    
    if(iRet < 0)
    {
        printf("fcntl failed msg:%s\n", strerror(errno));
        iRet = ERR_LOCK;
    }
    
    close(fd);
    
    return iRet;
    
}


/****************************************************
* @brief ��ȡ���һ�μ�ֵ
* @param[in] dev: �豸����
* @param[in] size: ��ȡ��ֵ����󻺳������ȣ� ��λΪ�ֽ�
* @param[out] key_val_list: ��ֵ�б�����linux�ں˶���ı�׼����ֵ��
* @note ���������ѭ������ԭ�� �����ӿ�ֻ��ȡ��һ����״ֵ̬�� 
         ����������ϰ���״̬��ƽ̨�������ṩ��
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
* @author: liuyue 2018-10-24
****************************************************/
static int32 key_get_value(struct tag_KEY_DEVICE *dev, struct tag_KEY_VALUE *key_val_list,
	uint32 size)
{

    int fd = 0;
    int retval = 0;
    fd_set fdset;
    int iRet = ERR_NORMAL;

    FD_ZERO (&fdset);                          /*��ռ��������е�Ԫ��*/    
    FD_SET(STDOUT_FILENO,&fdset);              /*����stdout��ʹ�����а���stdout*/ 

    if((NULL == dev)||(NULL == key_val_list))
    {
        printf("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return ERR_PTR;
    }
    
    fd = open(KEY_DEV_NAME, O_RDONLY|O_NONBLOCK);
    if(fd < 0) 
    {
        printf("Error opening %s device\n", KEY_DEV_NAME);
        return ERR_NORMAL;
	}

    if(flock(fd, LOCK_EX))
    {
		printf("key lock Error");
		goto end;
    }

    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);

    retval = select(fd + 1, &fdset, NULL, NULL, NULL);
    if (retval < 0) 
    {
	 DEBUG("KEY select error\n");
	 goto end;
    }    
	
    if(retval == 0) 
    {
    	 DEBUG("timeout.\n");
        goto end;
    } 

	/*�����ǰ�����ݿɶ����豸�ǰ��������ȡ��ֵ����ʾ*/
    if(FD_ISSET(fd, &fdset)) 
    {
        retval = read(fd, &(key_val_list->key), sizeof(key_val_list->key));
        if(0 >= retval) 
        {
            //printf("%s:%d read key error, ret=%d\n", __FUNCTION__, __LINE__, retval);
            goto end;
        }

    	if(key_val_list->key & KEYRELEASE)
    		key_val_list->status = 0;
    	else
    		key_val_list->status = 1;

        key_val_list->key &= 0x7F; 

        printf("key:%02x status=%02x\n", key_val_list->key, key_val_list->status);
        key_press_time_write();

	    iRet = ERR_OK;
    }
    
end:
    flock(fd, LOCK_UN);
    close(fd);
	
    return iRet;
}
/**
* @brief ��ȡ���һ�ΰ���ʱ��ϵͳ����ʱ��
* @param[in] dev: �豸����
* @param[out] lst_press_time: ���һ�ΰ���ʱ��ϵͳ����ʱ�䣨ϵͳ����ʱ���ϵͳʱ���Է���ʱӰ�죬��λΪ���롱��
* @note ����APP��ȡ���һ�ΰ���ʱ��ϵͳ����ʱ���뵱ǰϵͳ����ʱ����бȶԣ�
        ����һ����ֵ������APPǿ���ջ���ʾ����Ȩ��
        ��key_value_get������ȡ����ֵʱˢ��lst_press_time��
        ������洢��/tmp/dev/lst_press_time�ļ��¹����нӿڹ���
* @return�ɹ�����ERR_OK��ʧ�ܷ��ش����룬��ֵδ�仯����һ�����
��ERR_NORMAL����
*/
static int32 key_press_time_get(struct tag_KEY_DEVICE *dev, uint32 *lst_press_time)
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;
    uint32 tmptime = 0;

    fd = open(KEY_PRESS_TIME_FILE, O_CREAT|O_RDONLY,0666);
    if(fd < 0)
    {
        printf("open %s error!\n", KEY_PRESS_TIME_FILE);
        return ERR_NORMAL;   
    }
    
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    
    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
        printf("fcntl error ret:%d\n", iRet);
    
    
    iRet = read(fd, &tmptime, sizeof(uint32) ); 
    if(iRet < 0)
    {
        printf("lst_press_time read error\n");
        iRet = ERR_NORMAL;
    }
    else
    {
        iRet = ERR_OK;
    }
    *lst_press_time = tmptime;
    
   
   lock.l_type = F_UNLCK;
   iRet = fcntl(fd, F_SETLK, &lock);
   
   if(iRet < 0)
   {
       printf("fcntl failed msg:%s\n", strerror(errno));
       iRet = ERR_LOCK;
   }
   
   close(fd);
   return iRet;
}   

static int key_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */
    
    KEY_DEVICE_T* dev = (KEY_DEVICE_T*)malloc(sizeof(KEY_DEVICE_T));
    if(dev == NULL)
    {
        printf("malloc failed \n");
        return ERR_MEM;
    }
	
    memset(dev, 0, sizeof(KEY_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_KEY;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->key_value_get = key_get_value;
    dev->key_press_time_get = key_press_time_get;
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int key_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    KEY_DEVICE_T *dev = (KEY_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* ��ʼ��ģ����� */
KEY_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "key",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-20",
        .nDevNum = KEY_DEV_NUM,
        .szDevices = aKeyDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = key_open,
        .pfClose = key_close,
    },
};
