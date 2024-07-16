#include <stdio.h>       //标准输入输出定义
#include <stdlib.h>      //标准函数库定义
#include <sys/ioctl.h>   //杂项操作
#include <string.h>

#include "common.h"
#include "font.h"
#include "lcd.h"

/**********************************************************************
* @name      : lcdctrl_help
* @brief     : 命令帮助
* @param[in] : 
* @param[out]: 
* @return    : 
* @Create    : 王津剑
* @Date      : 2022-4-15
* @Update    : 
**********************************************************************/
void lcdctrl_help()
{
    printf("Usage: lcdctrl <X> <Y> <space> <type> <date>\n");
    printf("  <X>:             X-axis for screen\n");
    printf("  <Y>              Y-axis for screen\n");
    printf("  <space>          line space\n");
    printf("  <type>:\n");
    printf("      -f           data type is file\n");
    printf("      -s           data type is string\n");
    printf("  <date>:\n");
    printf("                   <type>=-f, filepath\n");
    printf("                   <type>=-s, string, show data in \" \"\n");
}

int main(int argc, char** argv)
{
    unsigned char *bitmap = NULL;
    int max_x = 0;
    int max_y = 0;
    int space = 0;
    char *showdata = NULL;
    int len = 0;
    int i = 0;
    //int j = 0;
    T_RECTANGLE tRect;

    if(6 != argc)
    {
        lcdctrl_help();
        return 0;
    }

    max_x = atoi(argv[1]) < 16 ? 16 : atoi(argv[1]);
    max_y = atoi(argv[2]) < 16 ? 16 : atoi(argv[2]);
    space = atoi(argv[3]) < 0 ? 0 : atoi(argv[3]);
    if(0 == strcmp(argv[4], "-f"))
    {
        FILE *fd;
        fd = fopen(argv[5], "r");
        if(NULL == fd)
        {
            printf("Open %s faild\n", argv[5]);
            return 0;
        }
        fseek(fd, 0, SEEK_END);
        len = ftell(fd);
        showdata = (char*)malloc(len + 1);
        if(NULL == showdata)
        {
            printf("Malloc faild, len = %d\n", len + 1);
            fclose(fd);
            return 0;
        }
        fseek(fd, 0, SEEK_SET);
        fread(showdata, 1, len, fd);
        fclose(fd);
        showdata[len] = 0;
    }
    else if(0 == strcmp(argv[4], "-s"))
    {
        len = strlen(argv[5]);
        showdata = (char*)malloc(len + 1);
        if(NULL == showdata)
        {
            printf("Malloc faild, len = %d\n", len - 1);
            return 0;
        }
        memcpy(showdata, argv[5], len);
        showdata[len] = 0;
        /* 过滤\\ \n */
        font_input_string(showdata, len, &len);
    }
    else
    {
        lcdctrl_help();
        return 0;
    }

    printf("Show data: \n%s\n", showdata);

    /* 显示内容写到点阵 */
    bitmap = (unsigned char *)malloc(max_x * max_y);
    if(NULL== bitmap)
    {
        printf("Malloc faild, len = %d\n", max_x * max_y);
        return 0;
    }
    memset(bitmap, 0, max_x * max_y);
    for(i = 0; i < max_y; i ++)
    {
        bitmap[i * max_x + max_x - 1] = 1;
    }
    len = strlen(showdata);
    font_string_map(space, showdata, len, bitmap, max_x, max_y);

    /*for(i = 0; i < max_y; i ++)
    {
        for(j = 0; j < max_x; j++)
        {
            printf("%s", 0 == bitmap[max_x * i + j] ? " " : "█");
        }
       
        printf("\n");
    }*/

    /* 显示刷新 */
    lcd_init();

    tRect.start.x = 0;
    tRect.start.y = 0;
    tRect.end.x = max_x - 1;
    tRect.end.y = max_y - 1;
    lcd_brush_main(&tRect, bitmap, max_x, max_y);
    /* 释放资源 */
    lcd_close();
    free(bitmap);
    free(showdata);
    return 0;
}