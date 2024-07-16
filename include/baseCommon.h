/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief��   ����ͷ�ļ�
* @date��    2018/10/24
* @history�� 
*********************************************************************
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
//-------------------��׼��ͷ�ļ�--------------------------------
#include <stdio.h>       //��׼�����������
#include <stdlib.h>      //��׼�����ⶨ��
#include <unistd.h>      //Unix ��׼��������
#include <sys/types.h>   //ϵͳ���Ͷ���
#include <sys/stat.h>    //ϵͳ״̬����
#include <fcntl.h>       //�ļ����ƶ���
#include <termios.h>     //PPSIX �ն˿��ƶ���
#include <errno.h>       //����Ŷ���
#include <dlfcn.h>       //��̬����ƶ���
#include <string.h>      //�ַ�����������
#include <sys/time.h>    //ʱ�亯��
#include <time.h>        //ʱ�亯��
#include <pthread.h>     //�̺߳����ⶨ��
#include <sys/ipc.h>     //�ź���
#include <signal.h>      //ϵͳ�źŶ���
#include <sys/file.h>    //�ļ�ϵͳ����
#include <semaphore.h>   //�ź���
#include <sys/sem.h>     //SYSTEM V�ź���
#include <linux/rtc.h>   //ϵͳʱ�����
#include <sys/shm.h>     //�����ڴ�
#include <sys/mman.h>    //�ڴ��������
#include <limits.h>      //�����������������ֵ�ĳ���
#include <dirent.h>      //Ŀ¼����
#include <stdarg.h>      //��׼����ͷ�ļ����Ժ����ʽ������������б���Ҫ˵����һ�����ͣ�va_list����3���꣨va_start
#include <assert.h>      //�ṩ���ԣ�assert(���ʽ)
#include <libgen.h>      //ģʽƥ�亯������
#include <ctype.h>       //�ַ����Ժ���
#include <getopt.h>      //���������в���
#include <netinet/in.h>  //INTERNET��ַ��
#include <arpa/inet.h>   //INTERNET����
#include <sys/socket.h>  //�׽���
#include <sys/wait.h>    //���̿���
#include <sys/select.h>  //Select����
#include <sys/reboot.h>  //����
#include <sys/msg.h>     //��Ϣ����
#include <math.h>        //������ѧ����
#include <sys/ioctl.h>   //�������
#include <sched.h>       //ִ�е���
#include <linux/netlink.h>//�ں�̬���û�̬����֮��������ݴ���������IPC����
#include <asm/types.h>    //��һЩ�������͵Ķ���
#include <linux/types.h>   //��һЩ�������͵Ķ���
#include <stdint.h>
#include <stdbool.h>
#include <sys/sysinfo.h>  //��ȡϵͳ�����Ϣ�Ľṹ��
#define LINUX
#endif


#include "dataCommon.h"     // ͨ�������ļ�
#include "errCode.h"        // ͨ�ô�����
#endif
