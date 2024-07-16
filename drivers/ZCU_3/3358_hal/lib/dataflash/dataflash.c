#include "types.h"
#include <dataflash.h>
#include "sysDefine.h"

/*dataflash �豸·�� */
#define   DATAFLASH_DEV_NAME   "/dev/dataflash" 

#define DF_DEV_NUM      1

static char *aDFDevice[DF_DEV_NUM] = {HW_DEVICE_ID_DATAFLASH};

/**
* @brief д����
* @param[in] dev: �豸����
* @param[in] buffer: ���ݻ���
* @param[in] offest: ��ʼƫ�Ƶ�ַ
* @param[in] length: д�����ݳ���
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 dataflash_write(struct tag_DATAFLASH_DEVICE *dev, uint8 *buffer, 
uint32 offest, uint32 length)
{
  return ERR_OK;
}
/**
* @brief ������
* @param[in] dev: �豸����
* @param[out] buffer: ������ָ��
* @param[in] offest: ��ʼƫ�Ƶ�ַ
* @param[in] length: ����������
* @return �ɹ����ض����Ĵ���0�����ݳ��ȣ�ʧ�ܷ���FALSE
*/
int32 dataflash_read(struct tag_DATAFLASH_DEVICE *dev, uint8* buffer, 
uint32 offest, uint32 length)
{
  return ERR_OK;
}
/**
* @brief ��������
* @param[in] dev: �豸����
* @param[in] offest: ��ʼƫ�Ƶ�ַ
* @param[in] length: ����������
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 dataflash_erase(struct tag_DATAFLASH_DEVICE *dev, uint32 offest, uint32 length)
{
  return ERR_OK;
}

/**
* @brief ��ȡ�ɲ������С
* @param[in] dev: �豸����
* @return �ɹ������豸�ɲ������С��ʧ�ܷ���FALSE
*/
int32 dataflash_ebsize_get(struct tag_DATAFLASH_DEVICE *dev)
{
  return ERR_OK;

}

/**
* @brief ��ȡ�豸��С
* @param[in] dev: �豸����
* @return �ɹ������豸��С��ʧ�ܷ���FALSE
*/
uint32 dataflash_devsize_get(struct tag_DATAFLASH_DEVICE *dev)
{
   return ERR_OK;

}


/**
* @brief ����оƬд����
* @param[in] dev: �豸����
* @param[in] wp_enable: 1: ��д������ 0: �ر�д����
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 dataflash_write_protect_enable(struct tag_DATAFLASH_DEVICE *dev, uint8 wp_enable)
{
  return ERR_OK;
}

static int dataflash_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */
    
    DATAFLASH_DEVICE_T* dev = (DATAFLASH_DEVICE_T*)malloc(sizeof(DATAFLASH_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(DATAFLASH_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_DATAFLASH;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->dataflash_write = dataflash_write;
    dev->dataflash_read = dataflash_read;
    dev->dataflash_erase = dataflash_erase;
    dev->dataflash_ebsize_get = dataflash_ebsize_get;
    dev->dataflash_devsize_get = dataflash_devsize_get;
    dev->dataflash_write_protect_enable = dataflash_write_protect_enable;
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int dataflash_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    DATAFLASH_DEVICE_T *dev = (DATAFLASH_DEVICE_T*)pDevice;
    
    free(dev);
    return 0;
}

/* ��ʼ��ģ����� */
DATAFLASH_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "dataflash",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-20",
        .nDevNum = DF_DEV_NUM,
        .szDevices = aDFDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = dataflash_open,
        .pfClose = dataflash_close,
    },
};
