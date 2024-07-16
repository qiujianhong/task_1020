#include <dlfcn.h>
#include <hal.h>
#include <dirent.h>
#include <string.h>

/*常量定义*/
#define HW_MODULE_MAXNUM         (64)    /* 一个模块内包含的最大设备数量 */
#define HW_LIB_FILE_NAME_MAXLEN  HW_DEVICE_ID_MAXLEN + 10 
#define HW_LIB_SYS_PATH          "/lib/hal_lib/"

/*模块信息结构*/
typedef struct tag_ModulesInfo
{
    char filelist[HW_MODULE_MAXNUM][HW_LIB_FILE_NAME_MAXLEN]; /* 库文件列表*/
    uint32 uModuleNum;                                        /* 库文件数目*/
    struct tag_HW_MODULE *pmodule[HW_MODULE_MAXNUM];          /* 模块指针 */
}HW_ModulesInfo;

/* 全局模块 */
static HW_ModulesInfo g_ModulesInfo = {{"","",},0,{0}};

/****************************************************
* @brief 获取对应库文件
* @param[in] pSrc: 文件名
* @param[in] pDeviceID: device id
* @return 成功返回0 
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
* @brief 获取DeviceID
* @param[in] pSrc: 文件名
* @param[in] pDeviceID: device id
* @return 成功返回0 
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
* @brief 从目录中获取文件名
* @param[in] dir:  目录
* @param[in] depth: 目录级
* @return 成功返回0 
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
* @brief 获取module
* @param[in] id: 指定device id
* @param[in] path: 指定HAL动态库的默认路径
* @param[in] module: 获取实际的模块指针
* @return 成功返回0 
****************************************************/
int hw_get_module(const char *id, const char *path, struct tag_HW_MODULE **module)
{
    int i = 0;
    int status = -EINVAL;
    void *handle = NULL;
    HW_MODULE *hmi = NULL;
    
    /* 使用dlopen打开path路径下存放的目标共享库，得到库文件的
    句柄handle */
    handle = dlopen(path, RTLD_NOW);
    if (handle == NULL) 
    {
        /* 出错，通过dlerror获取错误信息 */
        char const *err_str = dlerror();
        DEBUG(" err=%s\n",  err_str);
        status = -EINVAL;
        goto done;
    }

	///DEBUG("dlopen ok\n");
    /* 获取实际模块的地址，其中HAL_MODULE_INFO_SYM_AS_STR固定定义为
    "HMI"，每个模块都一致，作为HAL的约束条件之一，详见HAL约束条
    件 */
    const char *sym = HAL_MODULE_INFO_SYM_AS_STR; 
    /* 使用dlsym找到符号为“HMI”的地址，这里即struct tag_HW_MODULE结
    构体的地址，并且赋给hmi */
    hmi = (HW_MODULE *)dlsym(handle, sym);
    if (hmi == NULL)
    {
      printf("\n dlsyn fail\n");
      status = -EINVAL;
      goto done;
    }
	
      DEBUG("%s %d\n", id, hmi->nDevNum);

    /* 检查模块id是否匹配 */
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
	
    /* 执行成功 */
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
        /* 返回得到的struct tag_HW_MODULE结构体的指针 */
        *module = hmi;
    }
	
    return status;
}

/****************************************************
* @brief 为上层提供的硬件访问入口，获取设备
* @param[in] device_id: 指定device id
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

	    /* 调用hw_get_module 函数，传参为device_id，
    hal_<module id>.so或hal_common.so 的默认存放路径，模块变量地址。
    通过module出参返回device_id对应的模块实例 */
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
    

    /* 调用模块挂接的pfOpen操作函数，通过device出参返回对应的设备实例，
    应用层通过返回的device指针即可操作该设备实例挂接的操作函数接口 */
    err = module->pfOpen(module, device_id, &device);
    if(err)
    {
        printf("%s:%d get device failed!\n", __FUNCTION__, __LINE__);
        return NULL;
    }
	
    return device;
}

/****************************************************
* @brief 为上层提供的硬件访问入口，释放设备
* @param[in] device: 设备指针
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
* @brief       hal库初始化
* @param[in]   无
* @param[out]  无
* @return      成功返回0; 失败返回小于0错误编号
* @note        函数的应用编程手册（详细描述函数的使用说明）
* @warning     注意事项
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
* @brief       驱动库退出 
* @param[in]   无
* @param[out]  无
* @return      成功返回0; 失败返回小于0错误编号
* @note        函数的应用编程手册（详细描述函数的使用说明）
* @warning     注意事项
*********************************************************************
*/
int hal_exit(void)
{
   memset(&g_ModulesInfo, 0x0, sizeof(g_ModulesInfo));
   printf("hal exit \n");

   return 0;
}


