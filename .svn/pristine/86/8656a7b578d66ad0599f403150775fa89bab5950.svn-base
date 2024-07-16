#include "systemCtrl.h"
#include "framecomm.h"
#include "hal.h"
#include "power.h"
#include "adc.h"
//#include <sys/socket.h>
#include        <sys/un.h>       /*sockaddr_un*/
#include        <netdb.h>            /**/
#include        <sys/uio.h>      /*uio*/
#include         <net/if.h>      /*ifi_xxx*/
#include        <linux/sockios.h>
#include        <unistd.h>
#include        <sys/reboot.h>
#include "smiOSConfig.h"

int system_reboot(void) 
{
    int i;
    char killCmd[128] = {0};
    const char *appName[appCnt] = {"mapManager","gui","wirelessDCM"};
    //const char *appInContainer[appInConCnt] = {"dbCenter","ccoRouter","taskManager","advPuAmr","advPdAmr"};
    //const char *containerName[appInConCnt] = {"baseContainer","baseContainer","edcContainer","advPuAmrContainer","advPdAmrContainer"};
    SMIOS_FMT_DEBUG("before reboot, kill all app!\n");
    for(i = 0; i < appCnt; i++)
    {
        memset(killCmd,0x0,sizeof(killCmd));
        sprintf(killCmd,"pkill -9 %s",appName[i]);
        if(system(killCmd) < 0)
        {
            SMIOS_FMT_DEBUG("%s:%s failed!\n",__FUNCTION__,killCmd);
        }
        usleep(1000);
    }
    for(i = 0; i < appInConCnt; i++)
    {
        //memset(killCmd,0x0,sizeof(killCmd));
        //sprintf(killCmd,"appm -S -c %s -n %s",containerName[i],appInContainer[i]);
        //if(system(killCmd) < 0)
        //{
        //    SMIOS_FMT_DEBUG("%s:%s failed!\n",__FUNCTION__,killCmd);
        //}
        usleep(1000);
    }

    SMIOS_FMT_DEBUG("[system_reboot]\n");
    
    sync();
    reboot(RB_AUTOBOOT); 
    
    return 1;
}

int act_after_secs(int sec, int (*fp)(void))
{
    int i;
    if(sec <= 0)
        return -1;
                        
    for(i = sec; i > 0 ; i--)
    {
        SMIOS_FMT_DEBUG("act after %d seconds\n",i);
        sleep(1);
    }
     return fp();
}

int system_get_power_state(sint32 *state)
{

#if (DISABLE_HAL == 0)
    //hal_init();
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        SMIOS_FMT_DEBUG("dev is null!\n");
        return -1;
    }
    SMIOS_FMT_TRACE("get power state!\n");
    if(dev->power_power_state_get(dev,POWER_BACKUP,state) < 0)
    {
        SMIOS_FMT_DEBUG("get power state failed!\n");
        return -1;
    }

    hal_device_release((HW_DEVICE *)dev);
#endif
    return 1;
}

int system_get_main_power_state(sint32 *state)
{
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        SMIOS_FMT_DEBUG("dev is null!\n");
        return -1;
    }
    if(dev->power_power_state_get(dev,POWER_MAIN,state) < 0)
    {
        SMIOS_FMT_DEBUG("get power state failed!\n");
        return -1;
    }
    hal_device_release((HW_DEVICE *)dev);
    return 1;
}

int system_battery_power_set(uint8 val)
{
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        SMIOS_FMT_DEBUG("dev is null!\n");
        return -1;
    }
    if(dev->power_battery_out_set(dev,0,val) < 0)
    {
        SMIOS_FMT_DEBUG("get power state failed!\n");
        return -1;
    }
    hal_device_release((HW_DEVICE *)dev);
    return 1;
}

int system_get_main_cell_power(int *value)
{
    int  ret = 0;
	ADC_DEVICE_T* dev = (ADC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ADC);
	if(NULL == dev)
	{
        SMIOS_FMT_DEBUG("dev is null!\n");
        return -1;
	}
	ret = dev->adc_volt_read(dev, EMADC_BATVOL_CHANNEL, value);
	hal_device_release((HW_DEVICE *)dev);
	return ret;
}

int system_get_ethernet_state(sint8 *deviceName, sint32 *state)
{
    UNUSED(deviceName);
    int err;
    struct ethtool_value edata;
    int allfail = 0;

    struct ifreq ifr;
    int fd;

    /* Setup our control structures. */
    memset( &ifr, 0, sizeof(ifr) );
    strcpy( ifr.ifr_name, deviceName);  //õôeth0eth1

    /* Open control socket. */
    fd = socket( AF_INET, SOCK_DGRAM, 0);

    if (fd < 0)
    {
        return 0;
    }

    edata.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = (caddr_t)&edata;
    err = ioctl(fd, SIOCETHTOOL, &ifr);//send_ioctl(fd, ifr);
    close( fd );

    if (err == 0)
    {
        //printf("  Link detected: %s %d\n",    edata.data ? "yes":"no",edata.data);
        allfail = edata.data;
    }
    
    *state = allfail;
    return 1;
}