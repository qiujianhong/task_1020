/*
**********************************************************
@文件名称: list.c
@功能说明: 链表操作
**********************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"


void list_head_init(LIST_HEAD_T *pList)
{    
    pList->next = pList;
    pList->prev = pList;
}

void list_add(LIST_HEAD_T *pNew, LIST_HEAD_T *pList)
{
    if(NULL == pNew || NULL == pList)
    {
        return;
    }

    pList->next->prev = pNew;
    pNew->next = pList->next;
    pNew->prev = pList;
    pList->next = pNew;
}

void list_add_tail(LIST_HEAD_T *pNew, LIST_HEAD_T *pList)
{
    if(NULL == pNew || NULL == pList)
    {
        return;
    }

    pList->prev->next = pNew;
    pNew->next = pList;    
    pNew->prev = pList->prev;
    pList->prev = pNew;    
}

void list_del(LIST_HEAD_T *pEntry)
{
    if(NULL == pEntry || NULL == pEntry->next || NULL == pEntry->prev)
    {
        return;
    }
    
    pEntry->next->prev = pEntry->prev;
    pEntry->prev->next = pEntry->next;
    
    pEntry->next = NULL;
    pEntry->prev = NULL;
}

