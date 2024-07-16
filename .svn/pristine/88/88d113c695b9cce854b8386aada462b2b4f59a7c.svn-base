#include <hal.h>
#include <power.h>

#define POWER_ON  1
#define POWER_OFF 0
int main(void)
{
    hal_init();
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*)hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }
#if 0
    dev->power_ply_vcc_out_set(dev, MODULE_1, POWER_OFF);
    sleep(10);
    dev->power_ply_vcc_out_set(dev, MODULE_1, POWER_ON); /* power on modules 1 */

    dev->power_ply_vcc_out_set(dev, MODULE_2, POWER_OFF);
    sleep(10);
    dev->power_ply_vcc_out_set(dev, MODULE_2, POWER_ON); /* power on modules 1 */
#endif
#if 0
    dev->power_ply_vcc_out_set(dev, MODULE_3, POWER_OFF);
    sleep(10);
    dev->power_ply_vcc_out_set(dev, MODULE_3, POWER_ON); /* power on modules 1 */

    dev->power_ply_vcc_out_set(dev, MODULE_4, POWER_OFF);
    sleep(10);
    dev->power_ply_vcc_out_set(dev, MODULE_4, POWER_ON); /* power on modules 1 */
#endif    
    dev->power_ply_vcc_out_set(dev, MODULE_2, POWER_OFF);
    sleep(10);
    dev->power_ply_vcc_out_set(dev, MODULE_2, POWER_ON); /* power on modules 1 */

    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
