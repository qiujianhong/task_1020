#include "types.h"
#include <acs.h>
#include "sysDefine.h"

/*acs �豸·�� */
#define   ACS_DEV_NAME      "/dev/acs0" 

#define ACS_DEV_NUM      1

static char *aAcsDevice[ACS_DEV_NUM] = {HW_DEVICE_ID_ACS};

/**
* @brief        ���ɱ��Ĵ���
* @param[in]     input_data: ��������
* @param[in]     input_len: �������ݳ���
* @param[out]    output_data: Ӧ���Ļ�����
* @param[in]     output_len: Ӧ���Ļ���������
* @return      �ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
*/
int32 acs_data_transfer(struct tag_ACS_DEVICE *dev, uint8 *input_data, int32 input_len, uint8 *output_data, int32 output_len)
{
    int32 iRet = ERR_NORMAL;

    return iRet;
	
}
int32 (*acs_data_transfer)(struct tag_ACS_DEVICE *dev, uint8 *input_data, int32 input_len, uint8 *output_data, int32 output_len);




static int32 acs_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */
    
    ACS_DEVICE_T* dev = (ACS_DEVICE_T*)malloc(sizeof(ACS_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_PTR;
    }
	
    memset(dev, 0, sizeof(ACS_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_ACS;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->acs_data_transfer = acs_data_transfer;
   
    *device = (HW_DEVICE*)dev;
    
    return 0;
}

static int acs_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    ACS_DEVICE_T *dev = (ACS_DEVICE_T*)pDevice;
    
    free(dev);
    return 0;
}

/* ��ʼ��ģ����� */
ACS_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "acs",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-18",
        .nDevNum = ACS_DEV_NUM,
        .szDevices = aAcsDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = acs_open,
        .pfClose = acs_close,
    },
};
