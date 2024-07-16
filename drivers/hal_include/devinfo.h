#ifndef _DEVINFO_H
#define _DEVINFO_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <string.h>
#include <hal.h>


#define HW_DEVICE_ID_DEVINFO   "devinfo"

/* 定义DEVINFO模块结构 */
typedef struct tag_DEVINFO_MODULE 
{
    HW_MODULE common;
}DEVINFO_MODULE;


typedef struct tag_DEVINFO_DEVICE
{
    struct tag_HW_DEVICE    base;
    
    /**
    * @brief��ȡappid
    * @param[in] appname: appӢ������
    * @param[out] appid: ��Ӧapp��ID��Ϣ
    * @param[in] max_appid_size: ���appid�Ļ�������С
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_read_appid)(struct tag_DEVINFO_DEVICE *dev, const char *appname, char *appid, int32 max_appid_size);

    /**
    * @briefдappid
    * @param[in] appname: appӢ������
    * @param[in] appid: ��Ӧapp��ID��Ϣ
    * @param[in] appid_size: appid�Ĵ�С
    * @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_write_appid)(struct tag_DEVINFO_DEVICE *dev, const char *appname, const char *appid, int32 appid_size);

    /**
    * @brief��ȡ�ں˴�С
    * @return�����ں˴�С��
    */
    int32 (*devinfo_get_kernel_size)(struct tag_DEVINFO_DEVICE *dev);

    /**
    * @brief ��ȡ�ں�����
    * @param[out] read_buf: ��ȡ�ں����ݵ�buffer
    * @param[in] read_len: ��ȡһ����С���ں�����
    * @param[in] offset: ��ȡ�ں����ݵ���ʼƫ��
    * @return�ɹ����ش���0�����ݳ��ȣ�ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_read_kernel_data)(struct tag_DEVINFO_DEVICE *dev, uint8 *read_buf, uint32 read_len, uint32 offset);

    /**
    * @brief��ȡbootloader��С
    * @return����bootloader��С��
    */
    int32 (*devinfo_get_bootloader_size)(struct tag_DEVINFO_DEVICE *dev);

    /**
    * @brief ��ȡbootloader����
    * @param[out] read_buf: ��ȡbootloader���ݵ�buffer
    * @param[in] read_len: ��ȡһ����С��bootloader����
    * @param[in] offset: ��ȡbootloader���ݶ�����ʼƫ��
    * @return�ɹ����ش���0�����ݳ��ȣ�ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_read_bootloader_data)(struct tag_DEVINFO_DEVICE *dev, uint8 *read_buf, uint32 read_len, uint32 offset);

    /**
    * @brief ��ȡBootLoaderǩ��id
    * @param[out] cert_id: ��ȡBootLoaderǩ��id�Ļ�����    cert_id = NULL,��ȡBootLoaderǩ��id�ĳ���
    * @return�ɹ����ش���0��BootLoaderǩ��id�ĳ��ȣ�ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_read_bootloader_cert_id)(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_id);

    /**
    * @brief ��ȡBootLoaderǩ��
    * @param[out] cert_buf: ��ȡǩ�����ݵĻ�����    cert_buf = NULL,��ȡbootloaderǩ������
    * @return�ɹ����ش���0������ǩ�����ȣ�ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_read_bootloader_sign)(struct tag_DEVINFO_DEVICE *dev, uint8 *cert_buf);

    /**
    * @brief ��ȡ�ں�ǩ��id
    * @param[out] cert_id: ��ȡ�ں�ǩ��id�Ļ�����    cert_id = NULL,��ȡ�ں�ǩ��id�ĳ���
    * @return�ɹ����ش���0���ں�ǩ��id�ĳ��ȣ�ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_read_kernel_cert_id)(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_id);

    /**
    * @brief ��ȡ�ں�ǩ��
    * @param[out] cert_buf: ��ȡ�ں�ǩ���Ļ�����    cert_buf = NULL,��ȡ�ں�ǩ���ĳ���
    * @return�ɹ����ش���0���ں�ǩ���ĳ��ȣ�ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_read_kernel_sign)(struct tag_DEVINFO_DEVICE *dev, uint8 * cert_buf);

    /** 
    * @brief ��ȡ APP �����Ϣ 
    * @param[in] appno: app ���
    * @param[out] appname: ��Ӧ app ������ 
    * @param[out] license: ��Ӧ app �������Ϣ 
    * @param[in] max_license_size: ��� license �Ļ�������С 
    * @return �ɹ����� ERR_OK��ʧ�ܷ��ش����롣 
    */
    int32 (*devinfo_read_applicense)(struct tag_DEVINFO_DEVICE *dev, const char *appno, char *appname, char *license, 
                                     int32 max_license_size); 

    /** 
    * @brief д app �����Ϣ 
    * @param[in] appno: app ��� 
    * @param[in] license: ��Ӧ app �������Ϣ 
    * @param[in] license_size: app �����Ϣ�Ĵ�С 
    * @return �ɹ����� ERR_OK��ʧ�ܷ��ش����롣 
    */ 
    int32 (*devinfo_write_applicense)(struct tag_DEVINFO_DEVICE *dev, const char *appno, const char *appname, 
                                      const char *license, int32 license_size);

    /** 
    * @brief ��ǰ app ������� 
    * @return ���ص�ǰapp�������
    */ 
    uint32 (*devinfo_applicense_count)( struct tag_DEVINFO_DEVICE *dev);

    /** 
    * @brief ����Ŷ�ȡ APP �����Ϣ 
    * @param[in] index: ��ţ���0��ʼ�� 
    * @param[out] appno: ��Ӧ app �ı�� 
    * @param[out] appname: ��Ӧ app ������
    * @param[out] license: ��Ӧ app �������Ϣ 
    * @param[in] max_license_size: ��� license �Ļ�������С 
    * @return �ɹ����� ERR_OK��ʧ�ܷ��ش����롣 
    */
    int32 (*devinfo_read_applicense_index)(struct tag_DEVINFO_DEVICE *dev, uint32 index, char *appno, char *appname, 
                                           char *license, int32 max_license_size);

    /**
    * @brief ��ȡ�ն��豸����
    * @return �ɹ����� ����0������ֵ��
              0x1NXXXXXX��ʾ���������նˣ����磺0x11XXXXXX��ʾI�ͼ�������0x12XXXXXX��ʾII�ͼ�������
              0x2NXXXXXX��ʾר�����նˣ����磺0x21XXXXXX��ʾI��ר�䣬0x22XXXXXX��ʾII��ר�䣬0x23XXXXXX��ʾIII��ר�䣩
              0x3NXXXXXX��ʾ��Դ���������նˣ����磺0x31XXXXXX��ʾ��Դ������ר�䣩
              ����XXXXXXΪ�����Զ��壬ʧ�ܷ��ش�����
    **/
    int32 (*devinfo_get_device_type)(struct tag_DEVINFO_DEVICE *dev);

    /**
    * @brief ��ȡ�ն�CPU�¶�
    * @return�ɹ������¶�ֵ����λ0.1�棻ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_get_cpu_temperature)(struct tag_DEVINFO_DEVICE *dev);

    /**
    * @brief ��ȡ������Ϣ
    * @return�ɹ�����0��ʧ�ܷ��ش����롣
    */
    int32 (*devinfo_get_vendor_info)(struct tag_DEVINFO_DEVICE *dev,char* szInfo, int infoSize);

    // �û���չ����  

}DEVINFO_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif
