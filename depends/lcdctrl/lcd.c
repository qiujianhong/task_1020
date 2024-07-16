/*
*********************************************************************
字库操作
*********************************************************************
*/
#include <stdio.h>       //标准输入输出定义
#include <stdlib.h>      //标准函数库定�?
#include <sys/ioctl.h>   //杂项操作
#include  <linux/fb.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include  "common.h"
#include  "lcd.h"

#define GPIO_DEV_NAME  "/dev/gpio"

#define GPIO_MAGIC        0xAA00
#define GPIO_OUT         (1<<4)
#define GPIO_IN         (0<<4)
#define GPIO_OUT_HIGH    (GPIO_MAGIC | GPIO_OUT | 1)
#define GPIO_OUT_LOW    (GPIO_MAGIC | GPIO_OUT | 0)

#define PIN_BASE        32
#define GPIO_BACKLIGHT  (PIN_BASE + 0x40 + 17)   /*lcd背光管脚 */

#define LCD_DEV_NAME    "/dev/fb0"    /*frame buffer */

/* 全局变量 */
unsigned char *pfb_buffer = NULL;
struct fb_var_screeninfo varScreenInfo ;
struct fb_fix_screeninfo fixScreenInfo ;
int    g_fb0_fd = 0;

/**********************************************************************
* @name      : set_gpio_value
* @brief     : 设置gpio
* @param[in] : int pin             引脚
               int value           �?
* @param[out]: 
* @return    : 0-成功/其他失败
* @Create    : 王津�?
* @Date      : 2022-4-16
* @Update    : 
**********************************************************************/
int set_gpio_value(int pin, int value)
{
    int iofd;
    int ret_value;
    
    //打开
    iofd = open(GPIO_DEV_NAME, O_RDONLY);
    if (iofd < 0)
    {
      printf("can not open gpio device.\n");
      return -1;
    }
    
    //(0 low | 1 high | <0 error)
    ret_value = ioctl(iofd, value, pin);
    close(iofd);
    return ret_value;
}

/**********************************************************************
* @name      : lcd_bk_set
* @brief     : 设置背光
* @param[in] : LCD_BKL_TYPE_E bkl         控制状�?
* @param[out]: 
* @return    : 0-成功/其他失败
* @Create    : 王津�?
* @Date      : 2022-4-16
* @Update    : 
**********************************************************************/
int lcd_bk_set(LCD_BKL_TYPE_E bkl)
{
    int ret = 0;
    ret = set_gpio_value(GPIO_BACKLIGHT, LCD_BKL_OFF == bkl ? GPIO_OUT_LOW : GPIO_OUT_HIGH);
    return ret < 0 ? -1 : 0;
}

/**********************************************************************
* @name      : lcd_brush_main
* @brief     : 刷新显示
* @param[in] : T_RECTANGLE *pRect             刷新区域
               unsigned char *pBitMap         显示点阵
               int size_x                     X轴长�?
               int size_y                     Y轴长�?
* @param[out]: 
* @return    : 0-成功/其他失败
* @Create    : 王津�?
* @Date      : 2022-4-18
* @Update    : 
**********************************************************************/
int lcd_brush_main(T_RECTANGLE *pRect, unsigned char *pBitMap, int size_x, int size_y)
{
    unsigned char *buffer = NULL;
    int i = 0;
    int j = 0;
    int X = 0;
    int Y = 0;
    int offset = 0;

    printf("RECTANGLE: (%d-%d), (%d-%d); X: %d, Y: %d\n", pRect->start.x, pRect->start.y, pRect->end.x, pRect->end.y, size_x, size_y);
    buffer = (unsigned char *)malloc(fixScreenInfo.smem_len);
    memcpy(buffer, pfb_buffer, fixScreenInfo.smem_len);

    for(i = 0; i < size_y; i++)
    {
        Y = pRect->start.y + i;
        if(Y >= pRect->end.y || Y >= varScreenInfo.yres)
        {
            break;
        }
        for(j = 0; j < size_x; j++)
        {
            X = pRect->start.x + j;
            if(X > pRect->end.x || X >= varScreenInfo.xres)
            {
                break;
            }
            offset = Y * varScreenInfo.xres + X;
            if(1 == pBitMap[i * size_x + j])
            {
                buffer[offset/8] &= ~(0x01 << (offset % 8));
            }
            else
            {
                buffer[offset/8] |= (0x01 <<  (offset % 8));
            }
        }
    }
    memcpy(pfb_buffer, buffer, fixScreenInfo.smem_len);

    /*for(i = 0; i < varScreenInfo.yres; i++)
    {
        for(j = 0; j < varScreenInfo.xres/8; j++)
        {
            printf(" %02X", pfb_buffer[i * varScreenInfo.xres / 8 + j]);
        }
        printf("\n");
    }*/
    free(buffer);
    usleep(400000);
    return 0;
}

/**********************************************************************
* @name      : lcd_init
* @brief     : LCD初始�?
* @param[in] : 
* @param[out]: 
* @return    : 0-成功/其他失败
* @Create    : 王津�?
* @Date      : 2022-4-16
* @Update    : 
**********************************************************************/
int lcd_init()
{
    int iRet = 0;

    g_fb0_fd  = open(LCD_DEV_NAME, O_RDWR);
    if(g_fb0_fd < 0)
    {
        printf("open %s faild\n", LCD_DEV_NAME);
        close(g_fb0_fd);
        return -1;
    }

    memset(&fixScreenInfo, 0x0, sizeof(fixScreenInfo));
    iRet = ioctl(g_fb0_fd , FBIOGET_FSCREENINFO, &fixScreenInfo);
    if(iRet < 0)
    {
        perror("ioctl");
        close(g_fb0_fd);
        return iRet;
    }

    memset(&varScreenInfo, 0x0, sizeof(varScreenInfo));
    iRet = ioctl(g_fb0_fd , FBIOGET_VSCREENINFO, &varScreenInfo);
    if(iRet < 0)
    {
        perror("ioctl");
        close(g_fb0_fd);
        return iRet;
    }

    printf("smem_len     = %d\n",fixScreenInfo.smem_len);
    printf("xres         = %d;  yres         = %d\n",varScreenInfo.xres, varScreenInfo.yres);
    printf("xres_virtual = %d;  yres_virtual = %d\n",varScreenInfo.xres_virtual, varScreenInfo.yres_virtual);
    
    pfb_buffer = mmap(NULL, fixScreenInfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, g_fb0_fd ,0);

    if(pfb_buffer == NULL)
    {
       close(g_fb0_fd ); 
       g_fb0_fd =-1;
       printf("pfb_buffer == NULL\n");
    }
    else
    {
        printf("pfb_buffer = %p\n", pfb_buffer);
    }
    lcd_bk_set(LCD_BKL_ON);
    memset(pfb_buffer, 0xFF, fixScreenInfo.smem_len);
    usleep(100000);
    return 0;
}

/**********************************************************************
* @name      : lcd_close
* @brief     : LCD关闭
* @param[in] : 
* @param[out]: 
* @return    : 0-成功/其他失败
* @Create    : 王津�?
* @Date      : 2022-4-16
* @Update    : 
**********************************************************************/
int lcd_close()
{
    /* unmap framebuffer*/
    munmap(pfb_buffer , fixScreenInfo.smem_len);

    if(g_fb0_fd > 0)
    {
        close(g_fb0_fd);
    }

    return 0;
}