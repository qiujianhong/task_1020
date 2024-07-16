/*
*********************************************************************
* @filename: trace_back.c
* @brief   : 回溯操作
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2020-6-17     create
*********************************************************************
*/
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stddef.h>
#include <sys/user.h>
#include <asm/perf_regs.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <pthread.h>

#include "task_log.h"

/* 最大支持函数深度16 */
#define SIGNAL_FUNCTION_MAX   16
#define ARM32_REG_NUM         16

extern void diag_sig_handler(int signo);

/* 异常日志路径 */
#if 0
void do_backtrace(int sig, siginfo_t* info, void* p)
{
    char           buf[2048] = {0};
    int            offset = 0;
    struct timeval tv;
    struct tm     *stime = NULL;

    int        i = 0;
    int        num = 0;
    void      *buffer[SIGNAL_FUNCTION_MAX];
    char     **strings;
    pid_t      tid = syscall(SYS_gettid);
    static int s_bFileOpen = 1;
    
    /* 第一次执行需要执行打开日志 */
    if(1 == s_bFileOpen)
    {
        create_sig_logfile();
        s_bFileOpen = 0;
    }

    gettimeofday(&tv, NULL);
    stime = localtime(&tv.tv_sec);

    sprintf(buf, "--------------------------------------------------\n"
                 "%04d-%02d-%02d %02d:%02d:%02d.%06ld\n"
                 "Signal: %s(%d) code(%d) Pid(%u) Uid(%u)\n"
                 "TID   : %d\n", 
                 stime->tm_year + 1900, stime->tm_mon + 1, stime->tm_mday, stime->tm_hour, stime->tm_min,
                 stime->tm_sec, tv.tv_usec, strsignal(info->si_signo), info->si_signo, info->si_code, info->si_pid, 
                 info->si_uid, (int)tid);
    add_sig_logfile(buf);

    /* 以下获取寄存器并不是出问题时候的, 等技术研究明确再打印寄存器的值 */
#if 0
    /* 获取寄存器 */ 
    unsigned long regs[ARM32_REG_NUM] = {0};
    register unsigned long reg_cpsr = 0;
    char *regname[ARM32_REG_NUM] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9",
                                        "R10", "R11", "R12", "SP", "LR", "PC"};
    int offset = 0;

    register unsigned long *unw_base asm ("r0") = regs;
    __asm__ __volatile__ (".align 2\nbx pc\nnop\n.code 32\n"
                          "stmia %[base], {r0-r15}\n" 
                          "orr %[base], pc, #1\nbx %[base]"
                          : [base] "+r" (unw_base) : : "memory", "cc");
    add_sig_logfile("Register:\n");
    for(i = 0; i < ARM32_REG_NUM; i++)
    {
        offset += sprintf(buf + offset, "%-4s: 0x%lx\n", regname[i], regs[i]);
    }

    __asm__ __volatile__ ("mrs %0, cpsr\n"
                          :: "r" (reg_cpsr): "memory", "cc");
    offset += sprintf(buf + offset, "CPSR: 0x%lx\n", reg_cpsr);
    add_sig_logfile(buf);
#endif

    /* 获取调用栈 */
    num = backtrace(buffer, SIGNAL_FUNCTION_MAX);
    strings = backtrace_symbols(buffer, num);
    if (strings == NULL) 
    {
        add_sig_logfile("\nFailed to get stack\n");
        exit(0);
        return;
    }

    offset = sprintf(buf, "\nbacktrace stack:\n");
    if(num > 1 && NULL != strstr(strings[0], "do_backtrace"))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for(; i < num; i++)
    {
        offset += sprintf(buf + offset, "%s\n", strings[i]);
    }
    offset += sprintf(buf + offset, "--------------------------------------------------\n");
    add_sig_logfile(buf);
    free(strings);
    exit(0);
}
#else
void do_backtrace(int sig)
{
    char       buf[2048] = {0};
    int        offset = 0;
    int        i = 0;
    int        num = 0;
    void      *buffer[SIGNAL_FUNCTION_MAX];
    char     **strings;
    pid_t      tid = syscall(SYS_gettid);

    if(sig != SIGDEADLOOP)
    {
        offset = sprintf(buf, "\n------------------Exception-----------------------\n"
                              "Signal: %s(%d) Pid(%u) TID(%d)\n", strsignal(sig), sig, getpid(), (int)tid);
    }
    else
    {
        offset = sprintf(buf, "\n------------------DeadLoop-----------------------\n"
                              "Signal: Pid(%u) TID(%d)\n", getpid(), (int)tid);
    }
    /* 以下获取寄存器并不是出问题时候的, 等技术研究明确再打印寄存器的值 */
#if 0
    /* 获取寄存器 */ 
    unsigned long regs[ARM32_REG_NUM] = {0};
    register unsigned long reg_cpsr = 0;
    char *regname[ARM32_REG_NUM] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9",
                                        "R10", "R11", "R12", "SP", "LR", "PC"};
    int offset = 0;

    register unsigned long *unw_base asm ("r0") = regs;
    __asm__ __volatile__ (".align 2\nbx pc\nnop\n.code 32\n"
                          "stmia %[base], {r0-r15}\n" 
                          "orr %[base], pc, #1\nbx %[base]"
                          : [base] "+r" (unw_base) : : "memory", "cc");
    add_sig_logfile("Register:\n");
    for(i = 0; i < ARM32_REG_NUM; i++)
    {
        offset += sprintf(buf + offset, "%-4s: 0x%lx\n", regname[i], regs[i]);
    }

    __asm__ __volatile__ ("mrs %0, cpsr\n"
                          :: "r" (reg_cpsr): "memory", "cc");
    offset += sprintf(buf + offset, "CPSR: 0x%lx\n", reg_cpsr);
    add_sig_logfile(buf);
#endif

    /* 获取调用栈 临时方法有缺陷, 改进 todo */
    num = backtrace(buffer, SIGNAL_FUNCTION_MAX);
    strings = backtrace_symbols(buffer, num);
    if (strings == NULL) 
    {
        offset += sprintf(buf + offset, "\nFailed to get stack\n");
        TASK_ERROR_LOG("%s", buf);
        printf("%s", buf);
        exit(0);
        return;
    }

    offset += sprintf(buf + offset, "\nbacktrace stack:\n");
    if(num > 1 && NULL != strstr(strings[0], "do_backtrace"))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for(; i < num; i++)
    {
        offset += sprintf(buf + offset, "%s\n", strings[i]);
    }
    offset += sprintf(buf + offset, "--------------------------------------------------\n");
    TASK_ERROR_LOG("%s", buf);
    printf("%s", buf);
    free(strings);

    if(sig != SIGDEADLOOP)
    {
        diag_sig_handler(sig);
        /* 死循环不在此处重启 */
        sleep(1);
        exit(0);
    }
}

#endif

/**********************************************************************
* @name      : init_sig_action
* @brief     ：初始化信息处理
* @param[in] ：char *filepath   异常日志路径, NULL则是程序路径下的ErrorSignal.log
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-17
* @Update    :
**********************************************************************/
int init_sig_action(char *filepath)
{
    char   curpath[256] = {0};
    char   filename[256] = {0};

    if (NULL == filepath || strlen(filepath) >= 256)
    {
        
        int cnt = readlink("/proc/self/exe", curpath, 256);
        if(cnt < 0 || cnt >= 256)
        {
            printf("wireless get err path!\n");
        }
        //获取当前目录绝对路径，即去掉程序名
        int i;
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }

        sprintf(filename, "%s%s", curpath, "Exception.log");
    }
    else
    {
        memcpy(filename, filepath, strlen(filepath));
    }
    task_log_init(filename);

#if 0
    if (sigemptyset(&act.sa_mask) == -1)
        return -1;

    if (sigaction(SIGILL, &act, NULL) == -1)
        return -1;

    if (sigaction(SIGABRT, &act, NULL) == -1)
        return -1;

    if (sigaction(SIGFPE, &act, NULL) == -1)
        return -1;

    if (sigaction(SIGSEGV, &act, NULL) == -1)
        return -1;

    if (sigaction(SIGUSR1, &act, NULL) == -1)
        return -1;

    if (sigaction(SIGBUS, &act, NULL) == -1)
        return -1;

    if (sigaction(SIGSYS, &act, NULL) == -1)
        return -1;
#else
    signal(SIGILL, do_backtrace);
    signal(SIGABRT, do_backtrace);
    signal(SIGFPE, do_backtrace);
    signal(SIGSEGV, do_backtrace);
    signal(SIGBUS, do_backtrace);
    signal(SIGSYS, do_backtrace);
    signal(SIGDEADLOOP, do_backtrace);
#endif

    signal(SIGPIPE, SIG_IGN);  //需要忽略TCP断开信号 防止异常

    return 0;
}

/**********************************************************************
* @name      : change_sig_file
* @brief     ：修改异常日志路径
* @param[in] ：char *filepath   异常日志路径, 长度小于256
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-18
* @Update    :
**********************************************************************/
void change_sig_file(char *filepath)
{
    task_log_init(filepath);
}

