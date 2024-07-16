/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   移植linux内核链表
* @date：    2019-11-29
* @history： 
* @User deatil:
1. 在数据结构中添加LIST_HEAD_T作为一个成员
2. 通过LIST_HEAD_T获取结构体, 使用
    LIST_GET_STRUCT(pos, type, member) 
3. 链表初始化
    void list_head_init(LIST_HEAD_T *pList);
4. 在链表头增加节点
    void list_add(LIST_HEAD_T *pNew, LIST_HEAD_T *pList);
5. 在链表尾增加节点
    void list_add_tail(LIST_HEAD_T *pNew, LIST_HEAD_T *pList);
6. 删除节点
    void list_del(LIST_HEAD_T *pEntry);
7. 遍历操作
    LIST_FOR_EACH(pos, head)
    LIST_FOR_EACH_SAFE(pos, n, head)
*********************************************************************
*/

#ifndef    __LIST_H_
#define __LIST_H_


#ifdef __cplusplus
    extern "C" {
#endif

typedef struct tagListHead 
{
    struct tagListHead *next, *prev;
}LIST_HEAD_T;

/*
* 判断链表是否为空
* head    LIST_HEAD_T 链表头
*/
#define LIST_IS_EMPTY(head)  (((LIST_HEAD_T *)(head))->next == (head))

/*
* 遍历
* pos     LIST_HEAD_T 指针
* head    LIST_HEAD_T 链表头
*/
#define LIST_FOR_EACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/*
* 安全遍历
* pos     LIST_HEAD_T 指针
* n       LIST_HEAD_T 临时指针
* head    LIST_HEAD_T 链表头
*/
#define LIST_FOR_EACH_SAFE(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

/* 
* 获取原始结构体
* pos     LIST_HEAD_T 指针
* type    结构体类型
* member  LIST_HEAD_T 成员名称
*/
#define LIST_GET_STRUCT(pos, type, member) (type *)((char *)pos - (char *)((type *)0)->member)

void list_head_init(LIST_HEAD_T *pList);
void list_add(LIST_HEAD_T *pNew, LIST_HEAD_T *pList);
void list_add_tail(LIST_HEAD_T *pNew, LIST_HEAD_T *pList);
void list_del(LIST_HEAD_T *pEntry);


#ifdef __cplusplus
}
#endif

#endif

