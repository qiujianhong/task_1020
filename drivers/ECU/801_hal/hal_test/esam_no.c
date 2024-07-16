#include "types.h"
#include <hal.h>
#include <esam.h>
#include "devinfo.h"
#include "../lib/include/sysDefine.h"

#define DATA_55H        				0x55
#define SW1_VAL         				0x90
#define SW2_VAL         				0x00
#define SW1_POS         				1
#define SW2_POS         				2
#define LEN1_POS        				3
#define LEN2_POS        				4
#define DATA_START_POS  				5
#define LCR_OFFSET      				5       /* data len + LCR_OFFSET = LCR pos */
#define LCR_LEN         				1       /* 1 byte */
#define UNLOCK                          0
#define ENLOCK                          1


uint8  CalLRC(uint8* pBuffer, int32 nLen)
{
        uint8 ch = 0x0;
        int32  i  = 0;

        if(pBuffer != NULL)
        {
                for(i=0; i<nLen; i++)
                   ch ^= pBuffer[i];

                ch = ~ch;
        }
        else
                printf("pBuffer is NULL\n");

        return ch;
}

static int32 CheckEsamFrame(uint8* pBuffer, int32 nLen)
{
   ////int32 head_pos = 0;
   int32 i = 0;
   uint8 * p;
   int32 iDataLen = 0;

   if( (pBuffer == NULL) || (nLen <= 0))
        return -1;

   p = pBuffer;
   
   for(i=0; i<nLen; i++)
   {
        if(pBuffer[i] == DATA_55H)
        {
               //// head_pos = i;
                if( (nLen-i) > 5)
                {
                        //printf("find i[%d]\n", i);
                        p = pBuffer+i;
                        break;
                }
                else
                {
                        //printf("Receive data error!\n");
                        return -1;
                }
         }
    }

    if((p[SW1_POS] == SW1_VAL) && (p[SW2_POS] == SW2_VAL))
    {
        iDataLen = (p[LEN1_POS]<<8) | p[LEN2_POS];
        /* add SW1 SW2 LEN1 LEN2 4 bytes */
        if( p[iDataLen + LCR_OFFSET] == CalLRC( &p[SW1_POS], (iDataLen + 4)) )
        {
                //printf("Receive data LCR  ok!\n");
                return ERR_OK;
        }
        
    }
    
    return -1;
}


int main(int argc, char** argv)
{
    uint8 aSendBuff[] = {0x55, 0x80, 0x36, 0x00, 0x02, 0x00, 0x00, 0x4b};//Get tesam
	uint8 aRecvBuff[256] = {0};
	int32  iRecvLen = 0x60;
	int32 iRet = -1;
	int32 iSendLen = sizeof(aSendBuff);
	//int32 iCount = 0;
	int32 dataLen = 0;
    //uint8 tmpBuf[10] = {0};
    int i = 0;

	hal_init();

    if(4 == argc)
    {
        DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
        if(dev)
        {
            dev->devinfo_write_applicense(dev, argv[1], argv[2], argv[3], strlen(argv[3]));
            printf("HAL name %s, no %s, lic %s\n", argv[2], argv[1], argv[3]);
            hal_device_release((HW_DEVICE *)dev);
        }
        
        return 0;
    }

    ESAM_DEVICE_T* dev = (ESAM_DEVICE_T*)hal_device_get(HW_DEVICE_ID_YX_ESAM);
    if(NULL == dev)
    {
        printf("dev is null\n");
        return -1;
    }

    while(iRet != ERR_OK)
    {
        iRet = dev->esam_set_lock(dev, ENLOCK);
        sleep(1);
    }
    dev->esam_set_power(dev, PWR_ON);
    dev->esam_set_lock(dev, UNLOCK);
    //printf("Send: ");
    //for(i = 0; i < iSendLen; i++)
    //{
        //printf("%02X ",aSendBuff[i]);
    //}
    //printf("\n");

    //printf("Recv: ");
    iRet = dev->esam_data_write(dev, aSendBuff, iSendLen);
    if(0 == iRet)
    {
        usleep(20000);

        iRecvLen = dev->esam_data_read(dev, aRecvBuff, 256);
        if(iRecvLen > 0)
        {
            //for(i = 0; i < iRecvLen; i++)
            //{
                //printf("%02X ",aRecvBuff[i]);
            //}
            //printf("\n");

            iRet = CheckEsamFrame(aRecvBuff, iRecvLen);
            //printf("ESAMNO: ");
            dataLen = aRecvBuff[4];
            if(0 == iRet && 8 == dataLen)
            {
                for(i = 0; i < dataLen; i++)
                {
                    printf("%02X",aRecvBuff[5 + i]);
                }
            }
        }
    }
    printf("\n");
    hal_device_release((HW_DEVICE *)dev);

	return 0;
} 
