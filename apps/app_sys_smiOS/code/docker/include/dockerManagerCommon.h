/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� dockerManagerCommon.h
           docker manager ͷ�ļ�
* @date�� 2019-12-2
*********************************************************************
*/

#ifndef __DOCKER_MANAGER_COMMON_H__
#define __DOCKER_MANAGER_COMMON_H__

#include "baseType.h"
#include "dockerManagerDefinition.h"
#include "appManagerDefinition.h"
#include "qlibc.h"
#include "msgHandler.h"

#ifdef __cplusplus
extern "C" {
#endif

sint32 docker_manager_init(void);
sint32 docker_manager_start(void);
sint32 docker_manager_exit(void);
void docker_manager_container_clear(void);
sint32 docker_cmd_execute(const char* cmd,  char* result);

sint32 docker_mangager_container_start(char* mirrorName, char* containerName);
sint32 docker_manager_docker_start(DOCKER_INFO_T* dockerInfo, char* containerName);
sint32 docker_manager_docker_stop(char* dockerId);
//sint32 docker_manager_docker_remove(char* dockerName, char* version);
//sint32 docker_manager_load(char* mirrorName);
sint32 docker_manager_mirror_download(char* url, char* mirrorName, char* md5);
sint32 docker_manager_mirror_update(char* url, char* mirrorName, char* md5Sum, UPGRADE_FILE_TYPE type, char* md5sum);
sint32 docker_manager_mirror_delete(char* mirrorName);
sint32 docker_manager_mirror_info_get(char* data, sint32* strLength);

qhashtbl_t* docker_manager_get_container_table_head(void);
qhashtbl_t* docker_manager_get_mirror_table_head(void);
//������װ����ѹ��
sint32 docker_package_decompress(char* fileName);
//�������ذ�У��
sint32 docker_download_package_check(char* fileName, char* md5Sum);
//������װ��У��
sint32 docker_install_package_check(char* fileName, char* sha1Sum);
//��������ǩ��У��
sint32 docker_digital_sign_check(char* fileName, char* containerName);
//������װ��ɾ��
sint32 docker_package_delete(char* packageName);
//���������ļ�����
sint32 docker_manager_parse_config(char* filename, DOCKER_INFO_T* dockerInfo, APP_START_INFO_T* appStartInfo);
//���������ļ�����
sint32 docker_manager_config_save(void);
//���������ļ�����
sint32 docker_manager_config_load(void);

//�����������
sint32 docker_manager_mirror_load(char* mirrorName);
//��ȡ������
sint32 docker_manager_container_name_get(char* containerNameArray, sint32* containerCount);
//��ȡ������cpuռ����
sint32 docker_manager_container_cpu_rate_get(const char* containerName, float* cpu);
//��ȡ������app����
sint32 docker_manager_container_app_name_get(const char* containerName, char* appNameArray, sint32* appCount);
//��ȡ������app����
sint32 docker_manager_app_name_get(const char* containerName, char* appNameArray, sint32* appCount);
//��ȡ�����ڴ�ռ����
sint32 docker_manager_container_mem_rate_get(const char* containerName, float* memory);
//��ȡ����falsh ռ����
sint32 docker_manager_container_flash_size_get(const char* containerName, char* flashSize);
//��ȡapp cpuʹ����
sint32 docker_manager_app_cpu_rate_get(const char* containerName, const char* appName, float* cpuRate);
//��ȡapp �ڴ�ռ����
sint32 docker_manager_app_mem_rate_get(const char* containerName, const char* appName, float* memRate);
//��ȡapp flashռ����
sint32 docker_manager_app_flash_size_get(const char* appName, char* flashSize);
//������Ϣ�ϱ�
sint32 docker_manager_info_report(char* data, sint32* length);

//������������߳�
void createDockerMonitorThread(void);

#ifdef DOCKER_FUNCTION_TEST
void createDockerTestThread(void);
#endif

typedef struct
{
    uint8 num;
    SERVICE_START_T info[2];
}SERVICE_START_INFO_T;
void service_stop_check();
int service_stop(uint8 type);
int service_start(SERVICE_START_T *serviceInfo);

extern char* shellcmd(const char* cmd, char* buff, int size);
#ifdef __cplusplus
}
#endif

#endif


