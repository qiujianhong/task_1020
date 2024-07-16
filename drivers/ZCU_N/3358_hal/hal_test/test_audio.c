#include <hal.h>
#include <audio.h>

int main(void)
{
    int cnt = 0;

	hal_init();
    AUDIO_DEVICE_T* dev = (AUDIO_DEVICE_T*)hal_device_get(HW_DEVICE_ID_AUDIO);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }

    printf("play audio index1\n");
    dev->audio_play(dev, EMAUDIO_INDEX_1);
    sleep(2);
    
	while(cnt++ < 2)
    {
        printf("bell off\n");
        dev->audio_set_buzzer_out(dev, 0, 1, 1);
        sleep(1);
        printf("bell on\n");
        dev->audio_set_buzzer_out(dev, 1, 1, 1);
        sleep(1);
	}
	
    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
