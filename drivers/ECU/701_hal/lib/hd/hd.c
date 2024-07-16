#include <dlfcn.h>
#include <hal.h>
#include <dirent.h>
#include <string.h>

/*��������*/
#define HW_MODULE_MAXNUM         (64)    /* һ��ģ���ڰ���������豸���� */
#define HW_LIB_FILE_NAME_MAXLEN  HW_DEVICE_ID_MAXLEN + 10 
#define HW_LIB_SYS_PATH          "/lib/hal_lib/"

/*ģ����Ϣ�ṹ*/
typedef struct tag_ModulesInfo
{
    char filelist[HW_MODULE_MAXNUM][HW_LIB_FILE_NAME_MAXLEN]; /* ���ļ��б�*/
    uint32 uModuleNum;                                        /* ���ļ���Ŀ*/
    struct tag_HW_MODULE *pmodule[HW_MODULE_MAXNUM];          /* ģ��ָ�� */
}HW_ModulesInfo;

/* ȫ��ģ�� */
static HW_ModulesInfo g_ModulesInfo = {{"","",},0,{0}};

/****************************************************
* @brief ��ȡ��Ӧ���ļ�
* @param[in] pSrc: �ļ���
* @param[in] pDeviceID: device id
* @return �ɹ�����0 
****************************************************/
static int32 GetFileFromDeviceID(char *pDevid, char* pFile)
{
    char *p = NULL;
    char id[100] = {0};

    if (pDevid == NULL || pFile == NULL )
    {
        printf("input pointer NULL\n");
        return -1;
    } else {
  	    strcpy(id,pDevid);
  	    p = strtok(id, "_");
        sprintf(pFile, "libhal_%s.so", p);
    //// printf(" file %s\n", pFile);
    }

    return 0;
}


/****************************************************
* @brief ��ȡDeviceID
* @param[in] pSrc: �ļ���
* @param[in] pDeviceID: device id
* @return �ɹ�����0 
****************************************************/
static int32 GetDeviceIDFromFileName(char *pSrc, char* pDeviceID)
{
    char *p1, *p2;

    p1 = strstr(pSrc, "libhal_");
    p2 = strstr(pSrc, ".so");

    if ((p1 == NULL) || (p2 == NULL) || (p1 > p2))
    {
        printf("Not found\n");
        return -1;
    } else {
        p1 += strlen("libhal_");
        memcpy(pDeviceID, p1, p2 - p1);
        DEBUG("%s %s %s\n",pSrc,p1, pDeviceID);
    }

    return 0;
}

/****************************************************
* @brief ��Ŀ¼�л�ȡ�ļ���
* @param[in] dir:  Ŀ¼
* @param[in] depth: Ŀ¼��
* @return �ɹ�����0 
****************************************************/
static int32 GetDirLibName(char *dir, int32 depth)
{
   int32 i= 0;
   DIR *dp;
   struct dirent *entry;
   struct stat statbuf;

   if ((dp = opendir(dir)) == NULL)
   {
       fprintf(stderr, "Cannot open directory: %s\n", dir);
       return -1;
   }

    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);

        if ((strcmp(".", entry->d_name) == 0) || (strcmp("..", entry->d_name) == 0)
		                      || (strcmp("libhd.so", entry->d_name) == 0))
        continue;

        ////printf("%s\n",  entry->d_name);
        if (strstr(entry->d_name, ".so") != NULL)
        {
           ///DEBUG("%s\n", entry->d_name);
           strcpy(g_ModulesInfo.filelist[i], entry->d_name);
           if(++i >= HW_MODULE_MAXNUM)
               break;
        }
    }

    closedir(dp);

    if(i>0)
  	    g_ModulesInfo.uModuleNum = i;

    DEBUG("uModuleNum %d\n", g_ModulesInfo.uModuleNum);

    return i;
}




/****************************************************
* @brief ��ȡmodule
* @param[in] id: ָ��device id
* @param[in] path: ָ��HAL��̬���Ĭ��·��
* @param[in] module: ��ȡʵ�ʵ�ģ��ָ��
* @return �ɹ�����0 
****************************************************/
int hw_get_module(const char *id, const char *path, struct tag_HW_MODULE **module)
{
    int i = 0;
    int status = -EINVAL;
    void *handle = NULL;
    HW_MODULE *hmi = NULL;
    
    /* ʹ��dlopen��path·���´�ŵ�Ŀ�깲��⣬�õ����ļ���
    ���handle */
    handle = dlopen(path, RTLD_NOW);
    if (handle == NULL) 
    {
        /* ����ͨ��dlerror��ȡ������Ϣ */
        char const *err_str = dlerror();
        DEBUG(" err=%s\n",  err_str);
        status = -EINVAL;
        goto done;
    }

	///DEBUG("dlopen ok\n");
    /* ��ȡʵ��ģ��ĵ�ַ������HAL_MODULE_INFO_SYM_AS_STR�̶�����Ϊ
    "HMI"��ÿ��ģ�鶼һ�£���ΪHAL��Լ������֮һ�����HALԼ����
    �� */
    const char *sym = HAL_MODULE_INFO_SYM_AS_STR; 
    /* ʹ��dlsym�ҵ�����Ϊ��HMI���ĵ�ַ�����Ｔstruct tag_HW_MODULE��
    ����ĵ�ַ�����Ҹ���hmi */
    hmi = (HW_MODULE *)dlsym(handle, sym);
    if (hmi == NULL)
    {
      printf("\n dlsyn fail\n");
      status = -EINVAL;
      goto done;
    }
	
      DEBUG("%s %d\n", id, hmi->nDevNum);

    /* ���ģ��id�Ƿ�ƥ�� */
    for (i = 0; i <hmi->nDevNum; i++)
    {
      if ( hmi->szDevices[i] == NULL)
	    break;

      if ((strcmp(id, hmi->szDevices[i]) == 0) ||(strstr(hmi->szDevices[i], id) != NULL))
      {
        status = 0;
	//// DEBUG("status = 0\n");
        break;
      }
    }
	
    if (i >= HW_MODULE_MAXDEVICES)
    {
      status = -EINVAL;
      goto done;
    }
	
    /* ִ�гɹ� */
    status = 0;
    
done:
    if (status != 0)
    {
      hmi = NULL;
      if (handle != NULL)
      {
        dlclose(handle);
        handle = NULL;
      }
    } 
    else 
    {
        /* ���صõ���struct tag_HW_MODULE�ṹ���ָ�� */
        *module = hmi;
    }
	
    return status;
}

/****************************************************
* @brief Ϊ�ϲ��ṩ��Ӳ��������ڣ���ȡ�豸
* @param[in] device_id: ָ��device id
*****************************************************/
struct tag_HW_DEVICE* hal_device_get(const char *device_id)
{
    int err = ERR_NORMAL;
    HW_MODULE *module = NULL;
    //const char *path;
    HW_DEVICE *device;
    uint32 i= 0, j = 0;
    int32 iRet = -1;
    
    if(NULL == device_id)
    {
        printf("%s:%d device id is null.\n", __FUNCTION__, __LINE__);
        return NULL;
    }

	    /* ����hw_get_module ����������Ϊdevice_id��
    hal_<module id>.so��hal_common.so ��Ĭ�ϴ��·����ģ�������ַ��
    ͨ��module���η���device_id��Ӧ��ģ��ʵ�� */
    for(i=0; i < g_ModulesInfo.uModuleNum; i++)
    {
       module = g_ModulesInfo.pmodule[i];
	for(j=0; j<module->nDevNum; j++)
	{
           if(0 == strcmp(module->szDevices[j], device_id))
           {
              iRet = 0;
              break;
           }
	}

	if(iRet == 0)
            break;
	else
	    module = NULL;
    }

    if(module == NULL)
    {
        printf("%s:%d device id < %s >not exist.\n", __FUNCTION__, __LINE__, device_id);
        return NULL;
    }
    

    /* ����ģ��ҽӵ�pfOpen����������ͨ��device���η��ض�Ӧ���豸ʵ����
    Ӧ�ò�ͨ�����ص�deviceָ�뼴�ɲ������豸ʵ���ҽӵĲ��������ӿ� */
    err = module->pfOpen(module, device_id, &device);
    if(err)
    {
        printf("%s:%d get device failed!\n", __FUNCTION__, __LINE__);
        return NULL;
    }
	
    return device;
}

/****************************************************
* @brief Ϊ�ϲ��ṩ��Ӳ��������ڣ��ͷ��豸
* @param[in] device: �豸ָ��
*****************************************************/
int32 hal_device_release(struct tag_HW_DEVICE* device)
{
    if(NULL == device)
    {
        printf("ptr is null \n");
        return -1;
    }
	
    device->pModule->pfClose(device->pModule, device);
    return 0;
}


/**
*********************************************************************
* @brief       hal���ʼ��
* @param[in]   ��
* @param[out]  ��
* @return      �ɹ�����0; ʧ�ܷ���С��0������
* @note        ������Ӧ�ñ���ֲᣨ��ϸ����������ʹ��˵����
* @warning     ע������
**********************************************************************/
int hal_init(void)
{
    int i = 0;
    int iErr = 0;
    int iRet = 0;
    char aDeviceID[HW_LIB_FILE_NAME_MAXLEN] = {0};
    char aPath[100] = {0};
    char aLibFile[100] = {0};
    struct tag_HW_MODULE *module;
  
    iRet = GetDirLibName(HW_LIB_SYS_PATH, 0);
    if(iRet <= 0)
    {
        DEBUG("hal_init get lib failed\n");
	 return ERR_NORMAL;
    }

    for(i=0; i<g_ModulesInfo.uModuleNum; i++)
    {    
        if(GetDeviceIDFromFileName(g_ModulesInfo.filelist[i], aDeviceID) != 0)
        {
            continue;
        }
	    if(GetFileFromDeviceID(aDeviceID, aLibFile) == 0)
	    {
	        sprintf(aPath, "%s%s", HW_LIB_SYS_PATH, aLibFile);
	    }
	    else
	    {
	 	    continue;
        }
	    DEBUG("%s\n", aPath);
	 
	    iErr = hw_get_module(aDeviceID, aPath, &module);
        if(iErr)
        {
            printf("%s:%d get module failed!\n", __FUNCTION__, __LINE__);
            return -2;
        }

	    g_ModulesInfo.pmodule[i] = module;

	    memset(aDeviceID, 0x0, sizeof(aDeviceID));
	    memset(aLibFile, 0x0, sizeof(aLibFile));
	    memset(aPath, 0x0, sizeof(aPath));
   }

   printf("hal modules init finished\n");

   return 0;
}


/**
*********************************************************************
* @brief       �������˳� 
* @param[in]   ��
* @param[out]  ��
* @return      �ɹ�����0; ʧ�ܷ���С��0������
* @note        ������Ӧ�ñ���ֲᣨ��ϸ����������ʹ��˵����
* @warning     ע������
*********************************************************************
*/
int hal_exit(void)
{
   memset(&g_ModulesInfo, 0x0, sizeof(g_ModulesInfo));
   printf("hal exit \n");

   return 0;
}


