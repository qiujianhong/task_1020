#include <bluetooth.h>
#include "sysDefine.h"

/*pulse �豸·�� */
#define   BLUETOOTH_DEV_NAME      "/dev/ttyS5" 

static char *aBTDevice[BT_DEVICE_NUM] = {HW_DEVICE_ID_BT};


/**
* @brief       �������ݷ���
* @param[in]     data: ��������
* @param[in]     len: �������ݳ���
* @return      �ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
*/
int32 bt_send(struct tag_BT_DEVICE *dev, uint8 *data, int32 len)
{
   return ERR_OK;
}

/**
* @brief       �������ݽ���
* @param[out]  data: ��������
* @param[in]   len:����������
* @return      �ɹ��������ݳ��ȣ�ʧ�ܷ��ش����롣�������¼1��
*/
int32 bt_recv(struct tag_BT_DEVICE *dev, uint8 *data, int32 len)
{

   return ERR_OK;
}

/**
* @brief       ����ģ����λ
* @return      �ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
*/
int32 bt_reset(struct tag_BT_DEVICE *dev)
{
  return ERR_OK;
}

/**
* @brief        ������������
* @param[in]    id: ��ϢID
* @param[in]    data: ���ò�������
* @return      �ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
*/
int32 bt_set_param(struct tag_BT_DEVICE *dev, HAL_BTINFO_E id, void* data)
{
   return ERR_OK;
}

/**
* @brief        ������ȡ��Ϣ
* @param[in]    id: ��ϢID
* @param[in]    len: ��������󳤶�
* @param[out]   info: ��Ϣ����
* @return      �ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
*/
int32 bt_get_param(struct tag_BT_DEVICE *dev, HAL_BTINFO_E id, void* info, int32 len)
{
   return ERR_OK;
}



/**
* @brief        ��ȡ��������״̬
* @param[in] 	dev: �豸����
* @return      �ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
*/
bool bt_is_connect(struct tag_BT_DEVICE *dev)
{
   return ERR_OK;
}


static int bluetooth_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */
    
    BT_DEVICE_T* dev = (BT_DEVICE_T*)malloc(sizeof(BT_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
	
    memset(dev, 0, sizeof(BT_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_BT;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->bt_send = bt_send;
	dev->bt_recv = bt_recv;
    dev->bt_reset = bt_reset;  
	dev->bt_set_param = bt_set_param;
    dev->bt_get_param = bt_get_param;
	dev->bt_is_connect = bt_is_connect;
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int bluetooth_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    BT_DEVICE_T *dev = (BT_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* ��ʼ��ģ����� */
BT_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "bluetooth",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-04-12",
        .nDevNum  = BT_DEVICE_NUM,
        .szDevices = aBTDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = bluetooth_open,
        .pfClose = bluetooth_close,
    },
};
