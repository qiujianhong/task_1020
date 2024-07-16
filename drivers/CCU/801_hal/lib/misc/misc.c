#include <misc.h>
#include "sysDefine.h"

/*misc �豸·�� */
#define   MISC_DEV_NAME      "/dev/misc" 

#define MISC_DEV_NUM      1
static char *aMiscDevice[MISC_DEV_NUM] = {HW_DEVICE_ID_MISC};

/**
* @brief ��ȡ�ļ�ϵͳ����״̬
* @param[in] dev: �豸����
* @param[in] fs_type: �ļ�ϵͳ���ͣ���ubifs/jiffs2�ȣ�
* @param[in] partition_idx:�����������
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 misc_fs_partition_state_get(struct tag_MISC_DEVICE *dev, int32 fs_type, int32 partition_idx)
{
  return ERR_OK;
}
/**
* @brief ����pwm����/�ر�
* @param[in] dev: �豸����
* @param[in] val: ����/�ر�����ֵ
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 misc_pwm_state_set(struct tag_MISC_DEVICE *dev, int32 val)
{
  return ERR_OK;
}

/**
* @brief ����pwm����
* @param[in] dev: �豸����
* @param[in] frq: pwmƵ��
* @param[in] duty: pwmռ�ձ�
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 misc_pwm_param_set(struct tag_MISC_DEVICE *dev, uint32 frq, uint32 duty)
{
  return ERR_OK;
}
/**
* @brief ��ȡpwm����
* @param[in] dev: �豸����
* @param[out] frq: pwmƵ��
* @param[out] duty: pwmռ�ձ�
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 misc_pwm_param_get(struct tag_MISC_DEVICE *dev, uint32 *frq, uint32 *duty)
{
  return ERR_OK;
}

/**        
* @brief ��ȡUSB������Ϣ
* @param[in] dev: �豸����
* @param[out] info: ��ȡ��Ϣ
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 misc_usb_info_get(struct tag_MISC_DEVICE *dev, uint32 *info)
{
  return ERR_OK;
}

/**
* @brief sim�����ȹ���
* @param[in] dev: �豸����
* @param[in] val: 0�� ֹͣ���ȣ� 1�� ��������
* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
*/
int32 misc_sim_heater_set(struct tag_MISC_DEVICE *dev, uint8 val)
{
  return ERR_OK;
}
// �û���չ����  


static int misc_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */
    
    MISC_DEVICE_T* dev = (MISC_DEVICE_T*)malloc(sizeof(MISC_DEVICE_T));
    if(dev == NULL)
    {
        return -1;
    }
    memset(dev, 0, sizeof(MISC_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_MISC;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->misc_fs_partition_state_get = misc_fs_partition_state_get;
    dev->misc_usb_info_get = misc_usb_info_get;
    dev->misc_pwm_param_get = misc_pwm_param_get;
    dev->misc_pwm_state_set = misc_pwm_state_set;
    dev->misc_usb_info_get = misc_usb_info_get;
    dev->misc_sim_heater_set = misc_sim_heater_set;
    
    
    *device = (HW_DEVICE*)dev;
    
    return 0;
}

static int misc_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    MISC_DEVICE_T *dev = (MISC_DEVICE_T*)pDevice;
    
    free(dev);
    return 0;
}

/* ��ʼ��ģ����� */
MISC_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "misc",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2018-10-18",
        .nDevNum = MISC_DEV_NUM,
        .szDevices = aMiscDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = misc_open,
        .pfClose = misc_close,
    },
};
