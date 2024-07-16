#include <hal.h>
#include <eeprom.h>
#include <string.h>

/*eeprom �豸·�� */
//#define EEPROM_DEV_FILE      "/sys/bus/i2c/devices/2-0050/eeprom"   /*eepromӳ��ɵ��ļ� */
//#define EEPROM_DEV_NAME      "/dev/i2c-0"   /*eeprom�豸 */
#define EEPROM_DEV_FILE        "/dev/eeprom"

#define EEPROM_DEV_NUM      1

static char *aEepromDevice[EEPROM_DEV_NUM] = {HW_DEVICE_ID_EEPROM};

/**********************************************************
* @brief д����
* @param[in] dev: �豸����
* @param[in] pBuffer: ���ݻ���
* @param[in] nOffest: ��ʼƫ�Ƶ�ַ
* @param[in] nLength: д�����ݳ���
* @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
***********************************************************/
static int32 eeprom_write(EEPROM_DEVICE_T *dev, uint8* pBuffer, 
          uint32 nOffest, uint32 nLength)
{
    int32 iRet = 0;
    int32 fd = 0;
    int32 i,n;
    int32 seg_cnt;    //��512�ֽ�Ϊһ�Σ�Ҫд�����ݷ�Ϊseg_cnt��
    int32 remainder;  //�ֶκ�ʣ����ֽ���
    
    //д���治��Ϊ��
    if (pBuffer == NULL) {
        DEBUG("Write buffer is NULL.\n");
        return ERR_PTR;
    }
    //д���ݵķ�Χ��8K����
    if (nOffest + nLength < 1 || nOffest + nLength > 8192) {
        DEBUG("Write eeprom out of range(should be in 8K).\n");
        return ERR_NORMAL;
    }

    fd = open(EEPROM_DEV_FILE, O_RDWR, 0666);
    if(fd == -1) 
    {
        DEBUG("eeprom dev open error\n");
        return ERR_NORMAL;
    }
    //����д��
    if( flock(fd, LOCK_EX) ){
       DEBUG("Lock Error");
	   iRet = ERR_LOCK;
        goto errexit;
    }

    usleep(10000);		

    seg_cnt = nLength / 512;
    remainder = nLength % 512;

    //ÿ512�ֽ�һд
    for (i = 0; i < seg_cnt; i++) 
    {
        //��λ
        if (lseek(fd, nOffest+(i*512), SEEK_SET) < 0) {
            DEBUG("eeprom locate error.\n");
	     iRet = ERR_IO;
            goto errexit;
        }
        //д��
        n = write(fd, pBuffer+(i*512), 512);
        if (n < 0) {
            DEBUG("eeprom write error.\n");
	     iRet = ERR_IO;
            goto errexit;
        } 
        else if (n<512 && n>=0){
            DEBUG("In segment %d write eeprom less then expectation.\n",i);
            break;
        }
        iRet += n;	//��¼ʵ�ʶ������ֽ�����Ϊ��������ֵ
    }

    usleep(10000);	
    //д�ֶκ��ʣ�ಿ�� 
    if ( (i == seg_cnt) ) 
    {
        //��λ
        if (lseek(fd, nOffest+(seg_cnt*512), SEEK_SET) < 0) {
            DEBUG("eeprom locate error.\n");
	     iRet = ERR_IO;
            goto errexit;
        }
        //д��
        n = write(fd, pBuffer+(seg_cnt*512), remainder);
        if (n  < 0) {
            DEBUG("eeprom write error.\n");
	     iRet = ERR_IO;
            goto errexit;
        } 
        else if (n<remainder && n>=0){
            DEBUG("In remainder write eeprom less then expectation.\n");
        }
		
        iRet += n;
    }

    flock(fd, LOCK_UN);
    close(fd);
	iRet = (iRet >= 0)?ERR_OK : ERR_NORMAL;
    return iRet;

errexit:
    flock(fd, LOCK_UN);
    close(fd);
    return  iRet;
}

/**********************************************************
* @brief ������
* @param[in] dev: �豸����
* @param[out] pBuffer: ������ָ��
* @param[in] nOffest: ��ʼƫ�Ƶ�ַ
* @param[in] nLength: ����������
* @return �ɹ����ض����Ĵ���0�����ݳ��ȣ�ʧ�ܷ��ش�����
***********************************************************/
static int eeprom_read(EEPROM_DEVICE_T *dev, uint8* pBuffer, 
             uint32 nOffest, uint32 nLength)
{
    int32 iRet = ERR_OK;
    int32 fd = 0;

	//�����治��Ϊ��
	if (pBuffer == NULL) 
    {
        printf("Eeprom read buffer is NULL.\n");
        return ERR_PTR;
    }
	
	//��ȡ������Ҫ��8K����
	if (nOffest + nLength < 1 || nOffest + nLength > 8192) 
    {
		printf("Read eeprom out of range.\n");
		return ERR_NORMAL;
	}

    fd = open(EEPROM_DEV_FILE, O_RDWR, 0666);
    if(fd == -1) 
    {
        printf("eeprom dev open error\n");
        return ERR_NORMAL;
    }
	
    if(flock(fd, LOCK_EX))
    {
        printf("eeprom lock Error");
		iRet = ERR_LOCK;
        goto errexit;
    }
    
	usleep(10000);	

	int i,n;
	int seg_cnt;	//��512�ֽ�Ϊһ�Σ�Ҫ�������ݷ�Ϊseg_cnt��
	int remainder;	//�ֶκ�ʣ����ֽ���
	seg_cnt = nLength / 512;
	remainder = nLength % 512;

	//ÿ512�ֽ�һ��ȡ
	for (i=0; i<seg_cnt; i++) 
	{
		//��λ
		if (lseek(fd, nOffest+(i*512), SEEK_SET) < 0) {
			printf("eeprom locate error.\n");
			goto errexit;
		}
		//��ȡ
		n = read(fd, pBuffer+(i*512), 512);
		if (n < 0) 
        {
			printf("eeprom read error.\n");
			goto errexit;
		} 
        else if (n<512 && n>=0)
        {
			printf("In segment %d read eeprom less then expectation.\n",i);
                        break;
		}
		
		iRet += n;	//��¼ʵ�ʶ������ֽ�����Ϊ��������ֵ 		
	}

	usleep(10000);
	//���ֶκ��ʣ�ಿ��
	if ( (i == seg_cnt) ) 
	{
		//��λ
		if (lseek(fd, nOffest+(seg_cnt*512), SEEK_SET) < 0) {
			printf("eeprom locate error.\n"); 
			goto errexit;
		}
		//��ȡ
		n = read(fd, pBuffer+(seg_cnt*512), remainder);
		if (n  < 0) { 
			printf("eeprom read error.\n");
			goto errexit;
		} 
        else if (n<remainder && n>=0){
			printf("In remainder read eeprom less then expectation.\n");
		}
		
		iRet += n;
	}

	flock(fd, LOCK_UN);
    close(fd);
	return iRet;

errexit:
    flock(fd, LOCK_UN);
    close(fd);
    return ERR_NORMAL;
}


/**********************************************************
* @brief ����оƬд����
* @param[in] dev: �豸����
* @param[in] wp_enable: 1: ��д������ 0: �ر�д����
* @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
***********************************************************/
static int eeprom_write_protect_enable(EEPROM_DEVICE_T *dev, 
    unsigned char wp_enable)
{
    int32 fd = 0;
    int32 iRet = ERR_OK;
    
	//�����������
	if (wp_enable != 1 && wp_enable != 0) {
    	DEBUG("WriteProtect param error(should be 1 or 0).\n");
    	return ERR_NORMAL;
	}

    fd = open(EEPROM_DEV_FILE, O_RDWR);
    if(fd == -1) 
    {
        DEBUG("eeprom dev open error\n");
        return ERR_NORMAL;
    }
    if(flock(fd, LOCK_EX))
    {
        DEBUG("Lock Error");
	   iRet = ERR_LOCK;
        goto end;
    }
    
	//д������ȥ����
    ioctl(fd, wp_enable);
    flock(fd, LOCK_UN);
end:

    close(fd);
    return iRet;
}


/**********************************************************
* @brief ��ȡ�豸��С
* @param[in] dev: �豸����
* @return �ɹ������豸��С��ʧ�ܷ���FALSE
***********************************************************/
static int eeprom_get_devsize(EEPROM_DEVICE_T *dev)
{
    return 8192;
}


static int eeprom_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_DEVICE����Ҳ��Ҫ������ */
    
    EEPROM_DEVICE_T* dev = (EEPROM_DEVICE_T*)malloc(sizeof(EEPROM_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
	
    memset(dev, 0, sizeof(EEPROM_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_EEPROM;
    dev->base.pModule = (HW_MODULE *)module;    

    dev->eeprom_write = eeprom_write;
    dev->eeprom_read = eeprom_read;
	dev->eeprom_devsize_get = eeprom_get_devsize;
    dev->eeprom_write_protect_enable = eeprom_write_protect_enable;
   
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int eeprom_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    EEPROM_DEVICE_T *dev = (EEPROM_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* ��ʼ��ģ����� */
EEPROM_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "eeprom",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-18",
        .nDevNum = EEPROM_DEV_NUM,
        .szDevices = aEepromDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = eeprom_open,
        .pfClose = eeprom_close,
    },
};

