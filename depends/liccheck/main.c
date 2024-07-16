#include <stdio.h>       //标准输入输出定义
#include <stdlib.h>      //标准函数库定义
#include <sys/ioctl.h>   //杂项操作
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

int app_check_online(char *appname)
{
    int ret = 0;
    char pstr_cmd[128] = {0};
    char pstatus[10] = {0};
    
    sprintf(pstr_cmd, "ps |grep %s |grep -v grep | wc -l ", appname);

    FILE* pipe = popen(pstr_cmd, "r");
    if (!pipe)  
    {
        pipe = popen(pstr_cmd, "r");
    }
    if (!pipe)
    {
        return 1;
    }
    
    fgets(pstatus, 10, pipe);
    pclose(pipe);
    if(pstatus[0] >= '1')
    {
        ret = 1;
    }
    
    return ret;
}

int SecReinf_uboot_failed_check()
{
    int  fd = 0;
    char bufread[100] = {0};
    int  ret = 0;
    
    fd = open("/data/app/SecReinf/log/SecReinf.log", O_RDONLY);
    if(fd < 0)
    {
        return 0;
    }

    lseek(fd, -80, SEEK_END);
    ret = read(fd, bufread, 80);
    close(fd);
    if(ret <= 0)
    {
        return 0;
    }

    printf("Last SecReinf log: %s\n", bufread);

    if(NULL != strstr(bufread, "verify uboot failed, exit secReinf"))
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}

/* 上电后检查是否uboot验签失败 */
int main(int argc, char** argv)
{
    int count = 0;
    int bSecReinf = 1;
    char cmd[1024] = {0};

    sleep(5);
    /* 1min内监测不到 SecReinf */
    while(1 == bSecReinf && count < 60)
    {
        bSecReinf = app_check_online("SecReinf");
        count++;
        sleep(1);
    }

    if(1 == bSecReinf)
    {
        printf("SecReinf ok\n");
        return 0;
    }

    if(1 == app_check_online("smiOS"))
    {
        printf("smiOS ok\n");
        return 0;
    }

    /* 检查是否uboot失败 */
    if(0 == SecReinf_uboot_failed_check())
    {
        printf("uboot ok\n");
        return 0;
    }

    printf("uboot failed\n");
    /* 显示界面提示 */
    sprintf(cmd, "lcdctrl 160 160 2 -s \"  uboot验签失败\n  解决方案:\n  1.执行秘钥恢复\n  2.重启终端\"");
    system(cmd);
    sleep(1);
    system(cmd);
    
    sleep(10);

    /* 使用非正式安全加固APP操作 */
    system("rm -rf /usr/sec-app/package/work/SecReinf");
    sleep(1);
    system("tar -xvf /backup/SecReinf.tar -C /usr/sec-app/package/work/");
    sleep(1);
    system("cp /backup/applist.xml /usr/sec-app/package/work/SecReinf/cfg/");
    sleep(1);
    system("chmod 755 -R /usr/sec-app/package/work/SecReinf");
    sleep(1);
    system("/usr/sec-app/package/work/SecReinf/bin/startup_app.sh");
    sleep(5);
    
    return 0;
}